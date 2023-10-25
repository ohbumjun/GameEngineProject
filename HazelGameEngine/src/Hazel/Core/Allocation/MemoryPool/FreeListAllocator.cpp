#include "hzpch.h"
#include "FreeListAllocator.h"
#include "Hazel/Utils/EngineUtil.h"
#include <limits>  /* limits_max */

#define INF int(1e9)

FreeListAllocator::FreeListAllocator(const size_t totalSize, FreeListAllocatorPlacementPolicy Policy)
	: MemoryPoolAllocator(totalSize),
	m_Policy(Policy)
{
}

FreeListAllocator::~FreeListAllocator()
{
	delete m_StartPtr;

	m_StartPtr = nullptr;
}

void* FreeListAllocator::Allocate(const size_t allocSize, const size_t alignment)
{
	const size_t allocHeaderSize = sizeof(FreeListAllocator::AllocationHeader);
	const size_t freeHeaderSize = sizeof(FreeListAllocator::FreeHeader);

	assert(allocSize >= sizeof(Node));
	assert(allocSize >= 8);

	// Free Memory Block 을 돌면서
	// 최적의 Free Memory Block 을 찾는다.
	std::size_t padding;
	Node* affectedNode = nullptr, * prevNode = nullptr;

	Find(allocSize, alignment, padding, prevNode, affectedNode);

	// 부족할시 메모리 추가할당하기 
	assert(affectedNode != nullptr && "Not Enough Memory");

	const size_t alignmentPadding = padding - allocHeaderSize;
	const size_t requiredSize = allocSize + padding;

	const size_t rest = affectedNode->data.blockSize - requiredSize;

	// split block into data block, and a free block of size 'rest'
	Node* newFreeNode = (Node*)((std::size_t)affectedNode + requiredSize);

	if (rest > 0)
	{
		newFreeNode->data.blockSize = rest;
		newFreeNode->next = nullptr;

		m_FreeList.insert(affectedNode, newFreeNode);
	}

	m_FreeList.remove(prevNode, affectedNode);

	// set up date block
	const std::size_t headerAddress = (size_t)affectedNode + alignmentPadding;
	const std::size_t dataAdderess = headerAddress + allocHeaderSize;

	FreeListAllocator::AllocationHeader* HeaderPtr = (FreeListAllocator::AllocationHeader*)headerAddress;

	HeaderPtr->blockSize = requiredSize;
	// headerAddress 로부터, padding 만큼 이전에 가면
	// 가장 마지막에 할당된 datablock 의 끝 위치가 나온다는 의미
	HeaderPtr->padding = alignmentPadding;

	m_Used += requiredSize;
	m_Peak = m_Used;

	return (void*)dataAdderess;
}

void FreeListAllocator::Free(void* ptr)
{
	// Insert It in a sorted position by the address number
	// - Data Address 
	const std::size_t currentAddresss = (std::size_t)ptr;
	const std::size_t headerAddress = currentAddresss - sizeof(FreeListAllocator::AllocationHeader);

	const FreeListAllocator::AllocationHeader* allocHeader((FreeListAllocator::AllocationHeader*)headerAddress);

	Node* freeNode = (Node*)(headerAddress);
	freeNode->data.blockSize = allocHeader->blockSize + allocHeader->padding;
	freeNode->next = nullptr;

	Node* iter = m_FreeList.m_Head;
	Node* iterPrev = nullptr;

	while (iter != nullptr)
	{
		if (ptr < iter)
		{
			m_FreeList.insert(iterPrev, freeNode);
			break;
		}
		iterPrev = iter;
		iter = iter->next;
	}

	m_Used -= freeNode->data.blockSize;

	// Merge Contiguous Nodes
	Coalescene(iterPrev, freeNode);
}

void FreeListAllocator::Init()
{
	if (m_StartPtr == nullptr)
		delete m_StartPtr;

	m_StartPtr = new byte[m_TotalSize];

	Reset();
}

void FreeListAllocator::Reset()
{
	m_Used = 0;
	m_Peak = 0;

	Node* firstNode = (Node*)m_StartPtr;
	firstNode->data.blockSize = m_TotalSize;
	firstNode->next = nullptr;

	m_FreeList.m_Head = nullptr;
	m_FreeList.insert(nullptr, firstNode);
}

void FreeListAllocator::Coalescene(Node* prevBlock, Node* freeBlock)
{
	if (freeBlock->next != nullptr &&
		(std::size_t)freeBlock + freeBlock->data.blockSize == (size_t)freeBlock->next)
	{
		freeBlock->data.blockSize += freeBlock->next->data.blockSize;
		m_FreeList.remove(freeBlock, freeBlock->next);
	}

	if (prevBlock != nullptr &&
		(size_t)prevBlock + prevBlock->data.blockSize == (std::size_t)freeBlock)
	{
		prevBlock->data.blockSize += freeBlock->data.blockSize;
		m_FreeList.remove(prevBlock, freeBlock);
	}
}

void FreeListAllocator::Find(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode)
{
	switch (m_Policy)
	{
	case FreeListAllocatorPlacementPolicy::FIND_FIRST:
	{
		FindFirst(allocSize, alignment, padding, prevNode, foundNode);
	}
	break;
	case FreeListAllocatorPlacementPolicy::FIND_BEST:
	{
		FindBest(allocSize, alignment, padding, prevNode, foundNode);
	}
	break;
	}
}

void FreeListAllocator::FindBest(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode)
{
	size_t smallestDiff = (size_t)INF;

	Node* bestBlock = nullptr;
	Node* prevBestBlock = nullptr;
	Node* iter = m_FreeList.m_Head;
	Node* iterPrev = nullptr;

	while (iter != nullptr)
	{
		padding = EngineUtil::CalculatePaddingWithHeader((size_t)iter, alignment,
			sizeof(FreeListAllocator::AllocationHeader));

		const size_t requiredSpace = allocSize + padding;

		if (iter->data.blockSize >= requiredSpace && (iter->data.blockSize - requiredSpace) < smallestDiff)
		{
			smallestDiff = iter->data.blockSize;
			bestBlock = iter;
			prevBestBlock = iterPrev;
		}
		iterPrev = iter;
		iter = iter->next;
	}

	prevNode = prevBestBlock;
	foundNode = bestBlock;
}

void FreeListAllocator::FindFirst(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode)
{
	Node* iter = m_FreeList.m_Head;
	Node* iterPrev = nullptr;

	while (iter != nullptr)
	{
		padding = EngineUtil::CalculatePaddingWithHeader((size_t)iter, alignment,
			sizeof(FreeListAllocator::AllocationHeader));

		const size_t requiredSpace = allocSize + padding;

		if (iter->data.blockSize >= requiredSpace)
			break;

		iterPrev = iter;
		iter = iter->next;
	}
	prevNode = iterPrev;
	foundNode = iter;
}

