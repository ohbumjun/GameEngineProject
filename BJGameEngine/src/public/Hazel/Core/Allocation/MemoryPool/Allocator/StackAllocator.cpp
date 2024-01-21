#include "hzpch.h"
#include "StackAllocator.h"
#include "Hazel/Utils/EngineUtil.h"

StackAllocator::StackAllocator(size_t totalSize) :
	MemoryPoolAllocator(totalSize),
	m_StartPtr(nullptr),
	m_Offset(0)
{
}

StackAllocator::~StackAllocator()
{
	// free(m_StartPtr);
	delete m_StartPtr;
}

void* StackAllocator::Allocate(const size_t allocSize, const size_t alignment)
{
	const size_t currentAddress = (size_t)m_StartPtr + m_Offset;

	// const size_t padding = CEngineUtil::CalculatePaddingWithHeader(currentAddress, alignment, sizeof(AllocationHeader));
	// size_t calculated_padding = CEngineUtil::CalculatePaddingWithHeader(m_Offset - , alignment, sizeof(AllocationHeader));
	size_t padding = Utils::EngineUtil::CalculatePaddingWithHeader(m_Offset, alignment, sizeof(AllocationHeader));

	// 의도대로 라면, m_StartPtr 초기 위치에
	// 메모리를 재할당해줘야 한다.
	// 하지만, malloc 을 이용해서, 특정 위치에 메모리를 동적 할당할 수 없다. (placement new X)
	// if (currentAddress + padding + allocSize > (size_t)m_StartPtr + m_TotalSize)
	if (m_Offset + padding + allocSize > m_TotalSize)
	{
		// 메모리 재할당
		ResizeAlloc();
	}

	const size_t nextAddres = currentAddress + padding;
	const size_t headerAddress = nextAddres - sizeof(AllocationHeader);

	vecAdds.push_back(nextAddres);

	AllocationHeader* allocheaderAddress = (AllocationHeader*)headerAddress;

	// AllocationHeader alloc(padding)
	// allocheaderAddress = &alloc;

	allocheaderAddress->padding = padding;

	m_Offset += (padding + allocSize);

	m_Used = m_Offset;
	m_Peak = std::max(m_Used, m_Peak);

	return (void*)nextAddres;
}

void StackAllocator::Free(void* ptr)
{
	// 1,2,3 순서대로 할당했다면
	// 3,2,1 순서대로 해제해야 한다.
	// 이미 Allocator::Free 에서 해당 메모리를 통해 만들어낸 Object 에 대한 소멸자는 호출해준 상태이다.
	// 여기서는 할당한 메모리 위치 및 Header 정보를 이용해서
	// m_Offset 정보를 적절하게 조정해나가면 되는 것이다.
	const std::size_t currentAddress = (std::size_t)ptr;
	const std::size_t headerAddress = currentAddress - sizeof(AllocationHeader);
	const AllocationHeader* allocationHeader{ (AllocationHeader*)headerAddress };

	size_t PrevOffset = m_Offset;

	m_Offset = currentAddress - allocationHeader->padding - (std::size_t)m_StartPtr;

	if (PrevOffset < m_Offset)
		assert(false);

	m_Used = m_Offset;

	m_Peak = std::max(m_Offset, m_Peak);
}

void StackAllocator::Init()
{
	// m_StartPtr = malloc(m_TotalSize);
	m_StartPtr = new byte[m_TotalSize];

	Reset();
}

void StackAllocator::Reset()
{
	m_Offset = 0;
	m_Peak = 0;
	m_Used = 0;
}

void StackAllocator::ResizeAlloc()
{
	// 현재 m_TotalSize 위치에 새롭게 추가적으로 메모리를 할당할 것이다.
	size_t NewTotalSize = m_TotalSize * 2;

	byte* NewAlloc = new byte[NewTotalSize];

	memcpy(NewAlloc, m_StartPtr, NewTotalSize);

	m_StartPtr = new (m_StartPtr) byte[NewTotalSize];

	memcpy(m_StartPtr, NewAlloc, NewTotalSize);

	m_TotalSize *= 2;
}
