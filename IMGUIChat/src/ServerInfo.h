#pragma once

// Network

#ifdef HZ_PLATFORM_WINDOWS
#endif

// Imgui
#include <backends/imgui_impl_glfw.h>
#include <imgui.h>

// Standard
#include <stdlib.h>
#include <stdio.h>

#define TEST_SERVER_PORT "9190" // PORT 번호 문자열 지정
#define TEST_SERVER_IP_ADDRESS "127.0.0.1" // IP 주소 문자열 지정

/*
* 멀티캐스트 ip address 범위 : 224.0.0.0 to 239.255.255.255.
*/
#define TEST_MULTICAST_IP_ADDRESS "224.0.0.0" // IP 주소 문자열 지정

enum class NetworkType
{
	SERVER,
    CLIENT,

    TCP_SERVER,
    TCP_CLIENT,

	ECO_TCP_SERVER,
	ECO_TCP_CLIENT,

	MULTICAST_RECEIVER,
    MULTICAST_SENDER,

	OVERLAPPED_SENDER,
    OVERLAPPED_RECEIVER
};
