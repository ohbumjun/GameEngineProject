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
    void Coalescene(Node* prevBlock,
        Node* freeBlock);
    void Find(const size_t allocSize, const size_t alignment,
        size_t& padding, Node*& prevNode,
        Node*& foundNode);
    void FindBest(const size_t allocSize, const size_t alignment,
        size_t& padding, Node*& prevNode,
        Node*& foundNode);
    void FindFirst(const size_t allocSize, const size_t alignment,
        size_t& padding, Node*& prevNode,
        Node*& foundNode);


    byte* m_StartPtr = nullptr;
    SinglyLinkedList<FreeHeader> m_FreeList;
    FreeListAllocatorPlacementPolicy m_Policy;
};

