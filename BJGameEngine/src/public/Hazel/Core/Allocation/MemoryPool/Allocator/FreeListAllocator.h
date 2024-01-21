#pragma once

#include "../MemoryPoolInfo.h"
#include "MemoryPoolAllocator.h"
#include "Hazel/Core/DataStructure/DoublyLinkedList.h"
#include "Hazel/Core/DataStructure/AVLTree.h"

class FreeListAllocator :
    public MemoryPoolAllocator
{
    friend class MemoryPool;

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
        char padding;    // 1byte -> 8 bit -> 256 까지 표현가능
    };

    typedef unsigned char byte;
    // typedef SinglyLinkedList<FreeHeader>::Node Node;
    typedef DoublyLinkedList<FreeHeader>::Node Node;

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
    void Init();
    void Reset();

private:
    void coalescene(Node* prevBlock, Node* freeBlock);
    void find(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode);
    void findBest(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode);
    void findFirst(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode);
    void findSpeed(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode);
    void insertNode(Node* prev, Node* current);
    void removeNode(Node* prev, Node* current);
    byte* m_StartPtr = nullptr;
    // FreeList 내 노드들은, 즉 노드포인터들은, 사실상 각 Block 의 시작 주소와 동일하다. 
    // SinglyLinkedList<FreeHeader> m_FreeList;
    DoublyLinkedList<FreeHeader> m_FreeList;
    FreeListAllocatorPlacementPolicy m_Policy;
    AVLTree<size_t, void*> m_SizeAVLTree;
};