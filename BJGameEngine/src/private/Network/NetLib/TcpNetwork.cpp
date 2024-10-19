#include <cstring>
#include <vector>
#include <deque>

#ifndef _WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#endif

#include "Network/NetLib/ILog.h"
#include "Network/NetLib/TcpNetwork.h"


namespace NServerNetLib
{
	TcpNetwork::TcpNetwork()
	{
		
	}
	
	TcpNetwork::~TcpNetwork() 
	{
		for (auto& client : m_ClientSessionPool)
		{
			if (client.pRecvBuffer) {
				delete[] client.pRecvBuffer;
			}
			
			if (client.pSendBuffer) {
				delete[] client.pSendBuffer;
			}
		}
	}

	NET_ERROR_CODE TcpNetwork::Init(const ServerConfig* pConfig, ILog* pLogger)
	{
		std::memcpy(&m_Config, pConfig, sizeof(ServerConfig));

		m_pRefLogger = pLogger;

		auto initRet = InitServerSocket();
		if (initRet != NET_ERROR_CODE::NONE)
		{
			return initRet;
		}
		
		auto bindListenRet = BindListen(pConfig->Port, pConfig->BackLogCount);
		if (bindListenRet != NET_ERROR_CODE::NONE)
		{
			return bindListenRet;
		}

		// 파일 디스크립터 모든 비트값 0으로 초기화
		FD_ZERO(&m_Readfds);
		FD_SET(m_ServerSockfd, &m_Readfds);
		
		auto sessionPoolSize = CreateSessionPool(pConfig->MaxClientCount + pConfig->ExtraClientCount);
			
		m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | Session Pool Size: %d", __FUNCTION__, sessionPoolSize);

		return NET_ERROR_CODE::NONE;
	}

	void TcpNetwork::Release()
	{
#ifdef _WIN32
		WSACleanup();
#endif
	}

	RecvPacketInfo TcpNetwork::GetPacketInfo()
	{
		RecvPacketInfo packetInfo;

		if (m_PacketQueue.empty() == false)
		{
			packetInfo = m_PacketQueue.front();
			m_PacketQueue.pop_front();
		}
				
		return packetInfo;
	}
		
	void TcpNetwork::ForcingClose(const int sessionIndex)
	{
		if (m_ClientSessionPool[sessionIndex].IsConnected() == false) {
			return;
		}

		CloseSession(SOCKET_CLOSE_CASE::FORCING_CLOSE, m_ClientSessionPool[sessionIndex].SocketFD, sessionIndex);
	}

	void TcpNetwork::Run()
	{
		// 원본을 보존하기 위해 read_set 변수를 사용한다.
		// select 함수를 호출하면 변화가 생긴 fd를 제외한 나머지 위치의
		// 비트들은 전부 0으로 초기화 된다.
		// 따라서 원본을 보존해야 한다.
		auto read_set = m_Readfds;

		//연결된 모든 세션을 write 이벤트를 조사하고 있는데 사실 다 할 필요는 없다. 이전에 send 버퍼가 다 찼던 세션만 조사해도 된다.
		auto write_set = m_Readfds;
		
		timeval timeout{ 0, 1000 }; //tv_sec, tv_usec

#ifdef _WIN32
		// 변경된 소켓 및 관찰대상이 있다면, 변경된 대상의 "개수"를 리턴한다.
		// 만약 리턴 결과가 0 이라면, 변경된 대상이 존재하지 않는다는 것을 의미한다.
		auto selectResult = select(0, &read_set, &write_set, 0, &timeout);
#else
		auto selectResult = select(m_MaxSockFD + 1, &read_set, &write_set, 0, &timeout);
#endif

		auto isFDSetChanged = RunCheckSelectResult(selectResult);
		if (isFDSetChanged == false)
		{
			return;
		}

		// Accept
		// FD_ISSET : 변화 여부 확인
		// -> server socket 이 수신할 데이터가 있다는 의미
		//    == 새로운 접속 요청이 있었다는 의미.
		//		 따라서 이 경우 연결을 수락해야 한다.
		if (FD_ISSET(m_ServerSockfd, &read_set))
		{
			NewSession();
		}
		
		RunCheckSelectClients(read_set, write_set);
	}

	bool TcpNetwork::RunCheckSelectResult(const int result)
	{
		if (result == 0)
		{
			return false;
		}
		else if (result == -1)
		{
		    //linux에서 signal을 핸드링하지 않았는데 시그날이 발생하면 select에서 받아서 결과가 -1이 나온다
			return false;
		}

		return true;
	}
	
	void TcpNetwork::RunCheckSelectClients(fd_set& read_set, fd_set& write_set)
	{
		for (int i = 0; i < m_ClientSessionPool.size(); ++i)
		{
			auto& session = m_ClientSessionPool[i];

			if (session.IsConnected() == false) {
				continue;
			}

			SOCKET fd = session.SocketFD;
			auto sessionIndex = session.Index;

			// 정보를 수신한 클라이언트 소켓이 있는지 체크하고
			// 해당 클라이언트 소켓이 수신한 데이터를 별도의 PacketQueue 에 저장한다.
			auto retReceive = RunProcessReceive(sessionIndex, fd, read_set);

			if (retReceive == false) {
				continue;
			}

			// 반대로 데이터를 전송할 준비가 되어 있는 클라이언트 소켓이 있는지 확인한다.
			RunProcessWrite(sessionIndex, fd, write_set);
		}
	}

	bool TcpNetwork::RunProcessReceive(const int sessionIndex, const SOCKET fd, fd_set& read_set)
	{
		// 자. 인자로 들어오는 fd 는 서버 소켓 X
		// 무조건 클라이언트 소켓이 된다.
		// 클라이언트 소켓이 수신한 데이터가 있는지 확인한다.
		if (!FD_ISSET(fd, &read_set))
		{
			return true;
		}

		/*
		 *	클라이언트 소켓 입장에서 데이터를 수신하게 한다.
		 *	참고 : TCP 는 데이터 경계가 존재한다.
		 *	따라서 상대방이 100을 보냈어도, 여러 개의 패킷으로 나뉘어져 전송될 수 있고
		 *	이로 인해 recv 를 여러번 호출해야 할 수도 있다.
		 *	즉, recv 를 호출해도 그 size 가 100이 아니라 10 이렇게 적은 data size 만을
		 *	읽어들일 수 있다는 것이다.
		 */

		/*
		 * ex 1) Client 가 접속을 하고 나서, Login 요청을 날리면, 여리고 들어온다.
		 * 이때 fd 는 접속된 Client 의 소켓이 된다.
		 * 즉, 서버가 가지고 있는 Client 소켓 입장에서는, 클라이언트 측이 보낸 "로그인 요청" 이라는
		 * 데이터를 수신할 준비가 되어 있다고 판단한다는 것이다.
		 */
		
		NET_ERROR_CODE ret = RecvSocket(sessionIndex);

		if (ret != NET_ERROR_CODE::NONE)
		{
			CloseSession(SOCKET_CLOSE_CASE::SOCKET_RECV_ERROR, fd, sessionIndex);
			return false;
		}

		// RecvSocket 을 통해 수신버퍼에 저장한 데이터를 읽어들이는 함수이다.
		ret = RecvBufferProcess(sessionIndex);

		if (ret != NET_ERROR_CODE::NONE)
		{
			CloseSession(SOCKET_CLOSE_CASE::SOCKET_RECV_BUFFER_PROCESS_ERROR, fd, sessionIndex);
			return false;
		}

		return true;
	}

	NET_ERROR_CODE TcpNetwork::SendData(const int sessionIndex, const short packetId, const short bodySize, const char* pMsg)
	{
		auto& session = m_ClientSessionPool[sessionIndex];

		auto pos = session.SendSize;
		int16_t totalSize = (int16_t)(bodySize + PACKET_HEADER_SIZE);

		if ((pos + totalSize) > m_Config.MaxClientSendBufferSize ) {
			return NET_ERROR_CODE::CLIENT_SEND_BUFFER_FULL;
		}
				
		// packetId == PACKET_ID
		PacketHeader pktHeader{ totalSize, packetId, (uint8_t)0 };

		// 일단 전송 버퍼 끝에 packet header 정보를 넣어준다.
		memcpy(&session.pSendBuffer[pos], (char*)&pktHeader, PACKET_HEADER_SIZE);

		// 그리고 그 이후에 Packet 정보를 넣어준다.
		if (bodySize > 0)
		{
			memcpy(&session.pSendBuffer[pos + PACKET_HEADER_SIZE], pMsg, bodySize);
		}

		session.SendSize += totalSize;

		return NET_ERROR_CODE::NONE;
	}

	int TcpNetwork::CreateSessionPool(const int maxClientCount)
	{
		for (int i = 0; i < maxClientCount; ++i)
		{
			ClientSession session;
			session.Clear();
			session.Index = i;
			session.pRecvBuffer = new char[m_Config.MaxClientRecvBufferSize];
			session.pSendBuffer = new char[m_Config.MaxClientSendBufferSize];
			
			m_ClientSessionPool.push_back(session);
			m_ClientSessionPoolIndex.push_back(session.Index);			
		}

		return maxClientCount;
	}

	int TcpNetwork::AllocClientSessionIndex()
	{
		if (m_ClientSessionPoolIndex.empty()) 
		{
			return -1;
		}

		int index = m_ClientSessionPoolIndex.front();
		m_ClientSessionPoolIndex.pop_front();
		return index;
	}

	void TcpNetwork::ReleaseSessionIndex(const int index)
	{
		m_ClientSessionPoolIndex.push_back(index);
		m_ClientSessionPool[index].Clear();
	}

	NET_ERROR_CODE TcpNetwork::InitServerSocket()
	{
#ifdef _WIN32
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(wVersionRequested, &wsaData);

		m_ServerSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_ServerSockfd < 0)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_CREATE_FAIL;
		}

		auto n = 1;
		if (setsockopt(m_ServerSockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n)) < 0)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_SO_REUSEADDR_FAIL;
		}
#else
		m_ServerSockfd = socket(PF_INET, SOCK_STREAM, 0);
		if (m_ServerSockfd < 0)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_CREATE_FAIL;
		}

		auto n = 1;
		if (setsockopt(m_ServerSockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n)) < 0)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_SO_REUSEADDR_FAIL;
		}
#endif

		return NET_ERROR_CODE::NONE;
	}

	NET_ERROR_CODE TcpNetwork::BindListen(short port, int backlogCount)
	{
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;						// IPV4 프로토콜
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);		// "any" IPv4 address on the local machine.
        server_addr.sin_port = htons(port);						// port number to network byte order		

        if (bind(m_ServerSockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        {
            return NET_ERROR_CODE::SERVER_SOCKET_BIND_FAIL;
        }

      
		auto netError = SetNonBlockSocket(m_ServerSockfd);
		if (netError != NET_ERROR_CODE::NONE)
		{
			return netError;
		}

		if (listen(m_ServerSockfd, backlogCount) == SOCKET_ERROR)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_LISTEN_FAIL;
		}

        m_MaxSockFD = m_ServerSockfd;

		m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | Listen. ServerSockfd(%I64u)", __FUNCTION__, m_ServerSockfd);
		return NET_ERROR_CODE::NONE;
	}

	NET_ERROR_CODE TcpNetwork::NewSession()
	{
		auto tryCount = 0; // 너무 많이 accept를 시도하지 않도록 한다.

		do
		{
			++tryCount;

            struct sockaddr_in client_adr;
#ifdef _WIN32
			auto client_len = static_cast<int>(sizeof(client_adr));

			// 연결요청 대기큐에 있는 클라이언트 연결에 대해 새로운 소켓을 만든다.
			auto client_sockfd = accept(m_ServerSockfd, (struct sockaddr*)&client_adr, &client_len);
#else
			auto client_len = sizeof(client_adr);
			auto client_sockfd = accept(m_ServerSockfd, (struct sockaddr*)&client_adr, (socklen_t*)&client_len);
#endif
			if (client_sockfd == INVALID_SOCKET)
			{
#ifdef _WIN32
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					// non-blocking socket 이 사용되고 있을 수 있다는 것을 알려주는 것.

					/*
					ioctsocekt(sock, FIONBIO, &opt) 함수는, FIONBIO. 즉 입출력 모두를 opt 로 설정한다는 것.
					opt 값이 1 이면 non-blocking mode 로 설정하고, 0 이면 blocking mode 로 설정한다.

					이후 accept 를 호출하면, 클라이언트 연결요청이 없을 때, accept 함수는 바로 리턴하게 된다.
					그리고 INVALID_SOCKET 을 리턴하게 된다.

					이후 WSAGetLastError() 함수를 통해 WSAEWOULDBLOCK 에러 코드를 얻게 된다.

					*/
					return NET_ERROR_CODE::ACCEPT_API_WSAEWOULDBLOCK;
				}
#else
if (errno == EWOULDBLOCK)
{
	return NET_ERROR_CODE::ACCEPT_API_WSAEWOULDBLOCK;
}
#endif
				m_pRefLogger->Write(LOG_TYPE::L_ERROR, "%s | Wrong socket cannot accept", __FUNCTION__);
				return NET_ERROR_CODE::ACCEPT_API_ERROR;
			}

			auto newSessionIndex = AllocClientSessionIndex();
			if (newSessionIndex < 0)
			{
				m_pRefLogger->Write(LOG_TYPE::L_WARN, "%s | client_sockfd(%I64u)  >= MAX_SESSION", __FUNCTION__, client_sockfd);

				// 더 이상 수용할 수 없으므로 바로 짜른다.
				CloseSession(SOCKET_CLOSE_CASE::SESSION_POOL_EMPTY, client_sockfd, -1);
				return NET_ERROR_CODE::ACCEPT_MAX_SESSION_COUNT;
			}

			// IPv4 와 IPv6 주소를 binary 형태에서 사람이 알아보기 쉬운 텍스트(human-readable text)형태로 전환
			char clientIP[MAX_IP_LEN] = { 0, };
			inet_ntop(AF_INET, &(client_adr.sin_addr), clientIP, MAX_IP_LEN - 1);

			SetSockOption(client_sockfd);

			SetNonBlockSocket(client_sockfd);
			
			// 클라이언트 연결 소켓도 관찰대상으로 삼는다.
			FD_SET(client_sockfd, &m_Readfds);

			//m_pRefLogger->Write(LOG_TYPE::L_DEBUG, "%s | client_sockfd(%I64u)", __FUNCTION__, client_sockfd);
			ConnectedSession(newSessionIndex, client_sockfd, clientIP);

		} while (tryCount < FD_SETSIZE);
		
		return NET_ERROR_CODE::NONE;
	}
	
	void TcpNetwork::ConnectedSession(const int sessionIndex, const SOCKET fd, const char* pIP)
	{
	    if(m_MaxSockFD < fd)
        {
			/*
			This line checks if the current file descriptor fd 
			(representing the new client socket) 
			is greater than the m_MaxSockFD value. 
			
			m_MaxSockFD likely keeps track of the highest file descriptor value used so far.
			If fd is greater, the following line updates m_MaxSockFD. 
			
			This might be useful for managing resources or 
			keeping track of the highest socket descriptor used.
			*/
            m_MaxSockFD = fd;
        }

		++m_ConnectSeq;

		// 현재 여결된 클라이언트 소켓에 대한 세션 정보를 세팅한다.
		auto& session = m_ClientSessionPool[sessionIndex];
		session.Seq = m_ConnectSeq;
		session.SocketFD = fd;
		memcpy(session.IP, pIP, MAX_IP_LEN - 1);

		++m_ConnectedSessionCount;

		AddPacketQueue(sessionIndex, (short)PACKET_ID::NTF_SYS_CONNECT_SESSION, 0, nullptr);

		m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | New Session. FD(%I64u), m_ConnectSeq(%d), IP(%s)", __FUNCTION__, fd, m_ConnectSeq, pIP);
	}

	void TcpNetwork::SetSockOption(const SOCKET fd)
	{
		linger ling;
		ling.l_onoff = 0;
		ling.l_linger = 0;
		/*
		https://velog.io/@jyongk/TCP-%EC%86%8C%EC%BC%93-%EC%98%B5%EC%85%98-SOLINGER
		SO_LINGER : 소켓 close 시 전송되지 않은 데이터를 어떻게 할 것인가.
		TCP 는 보통 연결이 끊긴 후에도 바로 소켓을 닫지 않는다.
		TIME_OUT 시간 동안 대기를 하는데, 이 TIME_OUT 상태의 시간을
		SO_LINGER 옵션을 통해 조절할 수 있다. (ex) 바로 닫게 할 수 있다)

		l_onoff == 0 :  이것은 Linger를 비활성화 하겠다는 것으로 소켓의 default 값이다. 
		*/
		setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));

		int size1 = m_Config.MaxClientSockOptRecvBufferSize;
		int size2 = m_Config.MaxClientSockOptSendBufferSize;
		
		// 출력 버퍼, 입력 버퍼 크기 수정
		setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&size1, sizeof(size1));
		setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&size2, sizeof(size2));
	}

	void TcpNetwork::CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sockFD, const int sessionIndex)
	{
		if (closeCase == SOCKET_CLOSE_CASE::SESSION_POOL_EMPTY)
		{
#ifdef _WIN32
			closesocket(sockFD);
#else
			close(sockFD);
#endif
			// FD_CLR : 현재 연결을 해제하는 소켓을 관찰대상에서 제외한다.
			FD_CLR(sockFD, &m_Readfds);
			return;
		}

		if (m_ClientSessionPool[sessionIndex].IsConnected() == false) {
			return;
		}

#ifdef _WIN32
		closesocket(sockFD);
#else
		close(sockFD);
#endif
		FD_CLR(sockFD, &m_Readfds);

		m_ClientSessionPool[sessionIndex].Clear();
		--m_ConnectedSessionCount;
		ReleaseSessionIndex(sessionIndex);

		AddPacketQueue(sessionIndex, (short)PACKET_ID::NTF_SYS_CLOSE_SESSION, 0, nullptr);
	}

	NET_ERROR_CODE TcpNetwork::RecvSocket(const int sessionIndex)
	{
		auto& session = m_ClientSessionPool[sessionIndex];
		SOCKET fd = static_cast<SOCKET>(session.SocketFD);

		if (session.IsConnected() == false)
		{
			return NET_ERROR_CODE::RECV_PROCESS_NOT_CONNECTED;
		}

		int recvPos = 0;
				
		// 해당 클라이언트가 수신할 데이터가 남아있다면
		if (session.RemainingDataSize > 0)
		{
			// session.pRecvBuffer 쪽으로 남은 데이터들을 복사
			// : make space for new data at the end of buffer
			memcpy(session.pRecvBuffer, &session.pRecvBuffer[session.PrevReadPosInRecvBuffer], session.RemainingDataSize);
			
			// recvPos : 새로운 데이터 ? 가 들어갈 공간의 시작 위치
			// recvPos 부터 새로운 데이터를 recv 를 통해 받아들일 것이다.
			recvPos += session.RemainingDataSize;
		}

		auto recvSize = recv(fd, &session.pRecvBuffer[recvPos], (MAX_PACKET_BODY_SIZE * 2), 0);

		if (recvSize == 0)
		{
			return NET_ERROR_CODE::RECV_REMOTE_CLOSE;
		}

		// 논블로킹 소켓이기 때문에 recv 함수를 호출한 시점에
		// 아직 아무 데이터도 입력 버퍼로 부터 읽어들이지 못했을 수 있다.
		if (recvSize < 0)
		{
#ifdef _WIN32
			auto netError = WSAGetLastError();
#else
			auto netError = errno;
#endif
			if (netError != WSAEWOULDBLOCK)
			{
				return NET_ERROR_CODE::RECV_API_ERROR; 
			}
			else 
			{
				return NET_ERROR_CODE::NONE;
			}
		}

		// 수신 버퍼에 남아있는 total size ? 를 반영하는 변수로 보인다.
		session.RemainingDataSize += recvSize;

		return NET_ERROR_CODE::NONE;
	}

	NET_ERROR_CODE TcpNetwork::RecvBufferProcess(const int sessionIndex)
	{
		auto& session = m_ClientSessionPool[sessionIndex];
		
		auto readPos = 0;
		const auto dataSize = session.RemainingDataSize;
		PacketHeader* pPktHeader;

		/*
		case 1)
		- packet_header_size : 2
		- readPos			 : 2
		- pPktHeader->TotalSize : 22
		- bodySize			 : 20
		- datasize			 : 6
		- datasize - readPos : 4
		- readPos			 : 0
		- break; -> PacketQueue 에 데이터를 추가하지 않는다.

		아직, 해당 패킷의 모든 정보가 수신되지 않은 상황이다.
		body 는 20 이 들어와야 하는데, 아직 6 바이트만 들어온 것이다. ?

		session.RemainingDataSize -= readPos(0);
		session.PrevReadPosInRecvBuffer = readPos(0);

		return return NET_ERROR_CODE::NONE;
		*/


		/*
		case 2)
		>> 1st loop 
		- packet_header_size : 2
		- pPktHeader->TotalSize : 22
		- readPos : 2
		- datasize : 35
		- datasize - readPos : 33
		- bodySize : 20
		=> Packet 에 새로운 값 추가
		   즉, 해당 Packet 에 대응되는 모든 정보가 들어왔다고 가정하고. 패킷 정보를
		   PacketQueue 에 추가한다.

		- readPos = 2 - 2 = 0
		- Packet : recvBuffer[0]
		- readPos += bodySize --> readPos : 22
		- 즉, 하나의 버퍼에서 그 다음 패킷 정보로 stream 을 이동시키는 것이다.

		>> 2nd loop ( 그 다음 패킷 정보로 넘어온 것이다)
		- packet_header_size : 2
		- pPktHeader->TotalSize : 20
		- readPos  : 22
		- datasize : 35
		- datasize - readPos : 13
		- bodySize : 18
		=> if (bodySize > (dataSize - readPos)) 조건 성립
		   readPos -= 2 --> readPos : 20
		   break; -> PacketQueue 에 데이터를 추가하지 않는다.

		session.RemainingDataSize -= readPos; -> session.RemainingDataSize : 35 - 22 = 13
		session.PrevReadPosInRecvBuffer = readPos; -> 22

		즉, 아직 해당 패킷에 대한 모든 정보가 수신되지 않았다고 판단하고 빠져나오는 것이다.
		*/
		
		// Packet Data 부분을 모두 읽을 때까지 반복한다.
		while ((dataSize - readPos) >= PACKET_HEADER_SIZE)
		{
			pPktHeader = (PacketHeader*)&session.pRecvBuffer[readPos];

			readPos += PACKET_HEADER_SIZE;

			int16_t bodySize = (int16_t)(pPktHeader->TotalSize - PACKET_HEADER_SIZE);

			if (bodySize > 0)
			{
				// bodySize > 0 라는 것은 valid packet 이라는 의미이다. valid packet 은 body size > 0 
				if (bodySize > (dataSize - readPos))
				{
					// data bodySize 가 remaining data 크기보다 크다면
					readPos -= PACKET_HEADER_SIZE;
					break;
				}

				if (bodySize > MAX_PACKET_BODY_SIZE)
				{
					// 더 이상 이 세션과는 작업을 하지 않을 예정. 클라이언트 보고 나가라고 하던가 직접 짤라야 한다.
					return NET_ERROR_CODE::RECV_CLIENT_MAX_PACKET;
				}
			}

			// 수신한 정보를 Packet 형태로 만들어서 Packet Queue 에 넣어준다.
			AddPacketQueue(sessionIndex, pPktHeader->Id, bodySize, &session.pRecvBuffer[readPos]);
			readPos += bodySize;
		}
		
		session.RemainingDataSize -= readPos;
		session.PrevReadPosInRecvBuffer = readPos;
		
		return NET_ERROR_CODE::NONE;
	}

	void TcpNetwork::AddPacketQueue(const int sessionIndex, const short pktId, const short bodySize, char* pDataPos)
	{
		RecvPacketInfo packetInfo;
		packetInfo.SessionIndex = sessionIndex;
		packetInfo.PacketId = pktId;
		packetInfo.PacketBodySize = bodySize;
		packetInfo.pRefData = pDataPos;

		m_PacketQueue.push_back(packetInfo);
	}

	void TcpNetwork::RunProcessWrite(const int sessionIndex, const SOCKET fd, fd_set& write_set)
	{
		if (!FD_ISSET(fd, &write_set))
		{
			return;
		}

		/*
		 * ex 1) 접속 요청
		 *
		 * Client 가 접속 요청을 하면, 서버 소켓은 데이터를 수신한 소켓이므로 read_set 에 포함된다.
		 * 그래서 NewSession 을 통해 새로운 Client 정보를 만들어낸다.
		 *
		 * 그 다음 Run Loop 에서, Client 소켓에 대해 여기로 들어오게 된다.
		 * 즉, Client Socket 이 데이터를 Write 할 중비가 되어 있다는 얘기가 된다.
		 *
		 * 하지만, 실제 해당 Client Session 에 대해서 send 한 내용이 없으므로
		 * send Size 는 0 이 되고, 결국 FlushSendBuff 함수에서는 아무것도 안하고 나오게 된다.
		 */
		auto retsend = FlushSendBuff(sessionIndex);

		if (retsend.Error != NET_ERROR_CODE::NONE)
		{
			// 음... FlushSendBuffer 함수에서 send 함수 호출 결과 0 또는 음수를 반환하면
			// SOCKET_CLOSE_CASE::SOCKET_SEND_ERROR 가 된다.
			// 0 혹은 음수. 를 리턴한다는 것은 무조건 에러라는 것인가..?
			CloseSession(SOCKET_CLOSE_CASE::SOCKET_SEND_ERROR, fd, sessionIndex);
		}
	}

	NetError TcpNetwork::FlushSendBuff(const int sessionIndex)
	{
		auto& session = m_ClientSessionPool[sessionIndex];
		SOCKET fd = static_cast<SOCKET>(session.SocketFD);

		if (session.IsConnected() == false)
		{
			return NetError(NET_ERROR_CODE::CLIENT_FLUSH_SEND_BUFF_REMOTE_CLOSE);
		}

		auto result = SendSocket(fd, session.pSendBuffer, session.SendSize);

		if (result.Error != NET_ERROR_CODE::NONE) 
		{
			// ex) SendSocket 함수 내부에서 NET_ERROR_CODE::SEND_SIZE_ZERO 을 리턴할 경우
			// 즉, 아무 데이터도 send 함수를 통해 전송하지 못한 경우에 해당한다.
			return result;
		}

		// 여기에 오는 경우는 result가 NET_ERROR_CODE::NONE 일 때이다.
		// ? session.SendSize 가 0 일 때도 일단 여기로 들어오는건가..?
		auto sendSize = result.Vlaue;

		if (sendSize < session.SendSize)
		{
			// 실제 전송하고자 한 데이터보다 적게 전송했다면
			// 나머지 남은 부분들을 session.pSendBuffer 앞쪽으로 당겨온다.
			memmove(&session.pSendBuffer[0],
				&session.pSendBuffer[sendSize],
				session.SendSize - sendSize);

			session.SendSize -= sendSize;
		}
		else
		{
			// 그게 아니라면 모두 전송했다고 판단한다.
			session.SendSize = 0;
		}
		return result;
	}

	NetError TcpNetwork::SendSocket(const SOCKET fd, const char* pMsg, const int size)
	{
		NetError result(NET_ERROR_CODE::NONE);

		fd_set rfds = m_Readfds;

		// 접속 되어 있는지 또는 보낼 데이터가 있는지
		if (size <= 0)
		{
			// 보낼 데이터가 없다면 그대로 리턴
			return result;
		}

		// 일부 데이터를 보낸다.
		// 단, 논블로킹 소켓 옵션을 사용하고 있기 때문에 
		// send 함수를 호출한 시점에 모든 데이터가 전송되지 않을 수 있다.
		// 리턴값 : 전송된 총 바이트수 (ex) size 에 명시한 숫자보다 작은 숫자 리턴 가능)
		result.Vlaue = send(fd, pMsg, size, 0);

		if (result.Vlaue <= 0)
		{
			result.Error = NET_ERROR_CODE::SEND_SIZE_ZERO;
		}

		return result;
	}

	NET_ERROR_CODE TcpNetwork::SetNonBlockSocket(const SOCKET sock)
	{
		unsigned long mode = 1;

		// sock 핸들이 참조하는 소켓의 입출력 모드를 넌블로킹 모드로 하겠다.
		/*
		https://woo-dev.tistory.com/194
		- 넌블로킹 모드 : 소켓 입출력 함수가 블로킹 되지 않고 바로 리턴되도록 하는 모드
		ex) accept, recv, send 함수는 원래 블로킹 함수.
		그런데 이러한 옵션을 사용하고 나면, accept 의 경우 해당 함수를 호출한 시점에
		클라이언트 연결 요청이 없다면 INVALID_SOCKET 을 리턴한다.

		- send , recv : 패킷 수신 여부와 관계없이 논블로킹 모드시 해당 함수들을 사용해도
		바로 리턴해버린다. 따라서 논블로킹 모드로 소켓을 사용할 때는 해당 소켓이
		read,write 할 수 있는 상태인지를 판별해주는 select 함수가 필요하다.
		
		- 음...send,recv 는 무조건 블로킹 함수가 아닌가..?

		참고 : FIONBIO == 입출력 모드 설정, mode : 1 은 논블로킹으로
		이후 서버 문지기 소켓에 대해 만약 아래 함수를 통해 논블로킹 모드로 설정하게 되면
		accept 함수를 호출했을 때, 클라이언트 연결 요청이 없다면 INVALID_SOCKET 을 리턴한다.

		그리고 accept 함수로 통해 만들어진 클라이언트 소켓도 역시 논블로킹 모드 속성을 지니게 된다.
		-- NewSession 함수 참고.
		*/
#ifdef _WIN32
		if (ioctlsocket(sock, FIONBIO, &mode) == SOCKET_ERROR)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_FIONBIO_FAIL;
		}
#else
		if (ioctl(sock, FIONBIO, &mode) == -1)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_BIND_FAIL;
		}
#endif
		return NET_ERROR_CODE::NONE;
	}
	
}