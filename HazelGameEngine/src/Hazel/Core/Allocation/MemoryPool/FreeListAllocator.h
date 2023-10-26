#pragma once

#include "MemoryPoolInfo.h"
#include "MemoryPoolAllocator.h"
#include "Hazel/Core/DataStructure/SinglyLinkedList.h"
#include "Hazel/Core/DataStructure/AVLTree.h"

class FreeListAllocator :
    public MemoryPoolAllocator
{
    friend class CMemoryPool;

    struct FreeHeader
    {
        // 해당 block의 크기가 몇인지
        // Find first, Find Best 등의 알고리즘을 수행할 때 해당 blockSize 로
        // 비교를 수행하게 된다.
        size_t blockSize;
    };
    struct AllocationHeader
    {
        size_t blockSize;
        char padding;
    };

    typedef unsigned char byte;
    typedef SinglyLinkedList<FreeHeader>::Node Node;

public:
    FreeListAllocator(const size_t totalSize, FreeListAllocatorPlacementPolicy Policy);
    ~FreeListAllocator();    
    void SetFreeListAllocatorPlacementPolicy(FreeListAllocatorPlacementPolicy Policy)
    {
        m_Policy = Policy;
    }
    virtual void* Allocate(const size_t allocSize,
        const size_t alignment);
    virtual void Free(void* ptr);
    virtual void Init();
    void Reset();
    
private:
    void coalescene(Node* prevBlock, Node* freeBlock);
    void find(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode);
    void findBest(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode);
    void findFirst(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode);

    byte* m_StartPtr = nullptr;
    // FreeHeader -> 각 Block 의 시작점 주소 ?? 가 Node * 형태로 들어간다...?
    SinglyLinkedList<FreeHeader> m_FreeList;
    FreeListAllocatorPlacementPolicy m_Policy;
};

