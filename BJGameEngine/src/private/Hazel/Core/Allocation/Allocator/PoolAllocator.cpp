#include "hzpch.h"
#include "Hazel/Core/Allocation/MemoryPool/Allocator/PoolAllocator.h"

PoolAllocator::PoolAllocator(const size_t totalSize, const size_t chunkSize)
	: MemoryPoolAllocator(totalSize),
	m_ChunkSize(chunkSize)
{
	assert(totalSize % chunkSize == 0);
	assert(totalSize % 8 == 0);
}

PoolAllocator::~PoolAllocator()
{
	free(m_StartPtr);
}

void* PoolAllocator::Allocate(const size_t allocateSize, const size_t alignment)
{
	assert(m_ChunkSize == allocateSize);

	// Node* PopNode = m_FreeList.pop();

	// if (m_StkAddress)
	// 
	// if (!PopNode)
	// {
	// 	size_t PrevTotalSize = m_TotalSize;
	// 
	// 	// 메모리 재할당
	// 	m_TotalSize *= 2;
	// 
	// 	void* NewStartPtr = malloc(m_TotalSize);
	// 
	// 	memcpy(NewStartPtr, m_StartPtr, PrevTotalSize);
	// 
	// 	m_StartPtr = NewStartPtr;
	// 
	// 	// Free List 공간 추가할당
	// 	size_t ExtraChunkSize = PrevTotalSize / m_ChunkSize;
	// 
	// 	for (size_t i = 0; i < ExtraChunkSize; ++i)
	// 	{
	// 		size_t address = PrevTotalSize + i * m_ChunkSize;
	// 		stkAddress.push(address);
	// 		// m_FreeList.push((Node*)address);
	// 	}
	// 
	// 	// PopNode = m_FreeList.pop();
	// 	PopNode = stkAddress
	// }
	// 

	if (m_StkAddress.empty())
		assert(false);

	// 현재 할당할 수 있는 주소값 정보
	size_t AllocAddress = m_StkAddress.top();

	m_StkAddress.pop();

	m_Used += m_ChunkSize;
	m_Peak = std::max(m_Used, m_Peak);

	return (void*)AllocAddress;
}

void PoolAllocator::Free(void* ptr)
{
	// Node* FreedMemory = (Node*)ptr;
	// Node* FreedMemory = reinterpret_cast<Node*>(ptr); //
	// Node* FreedMemory = new ((Node*)ptr) Node();

	// m_FreeList 를 사용하면 안된다.
	// m_FreeList.push(FreedMemory);
	m_StkAddress.push((size_t)ptr);

	m_Used -= m_ChunkSize;
	m_Peak = std::max(m_Used, m_Peak);
}

void PoolAllocator::Init()
{
	m_StartPtr = malloc(m_TotalSize);

	Reset();
}

void PoolAllocator::Reset()
{
	size_t chunkN = m_TotalSize / m_ChunkSize;

	for (size_t i = 0; i < chunkN; ++i)
	{
		size_t address = (size_t)m_StartPtr + i * m_ChunkSize;

		m_StkAddress.push(address);
		// Node* NewNode = (Node*)address;
		// NewNode->Next = nullptr;
		// 
		// m_FreeList.push((Node*)address);
	}
}
