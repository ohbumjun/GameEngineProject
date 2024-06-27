#pragma once

#include "Hazel/Core/Allocation/MemoryPool/MemoryPoolInfo.h"
#include "Hazel/Core/DataStructure/AVLTree.h"
#include "Hazel/Core/DataStructure/DoublyLinkedList.h"
#include "Hazel/Core/Allocation/Allocator/Allocable.h"
namespace Hazel
{

class FreeListAllocator : public Allocable
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
        char padding; // 1byte -> 8 bit -> 256 까지 표현가능
    };

    typedef unsigned char byte;
    // typedef SinglyLinkedList<FreeHeader>::Node Node;
    typedef DoublyLinkedList<FreeHeader>::Node Node;

#pragma region Scope
    struct Scope
    {
        Scope(size_t totalSize, FreeListAllocatorPlacementPolicy policy);
        ~Scope();

        bool HasEnoughSpace(const size_t allocSize, const size_t alignment);
        bool Contain(void *addressPtr, size_t scopeSize));
        void *Alloc(const size_t allocSize);
        void Free(void *ptr);

    private:
        void find(const size_t allocSize,
                  const size_t alignment,
                  size_t &padding,
                  Node *&prevNode,
                  Node *&foundNode);
        void findBest(const size_t allocSize,
                      const size_t alignment,
                      size_t &padding,
                      Node *&prevNode,
                      Node *&foundNode);
        void findFirst(const size_t allocSize,
                       const size_t alignment,
                       size_t &padding,
                       Node *&prevNode,
                       Node *&foundNode);
        void findSpeed(const size_t allocSize,
                       const size_t alignment,
                       size_t &padding,
                       Node *&prevNode,
                       Node *&foundNode);

        void coalescene(Node *prevBlock, Node *freeBlock);
        void insertNode(Node *prev, Node *current);
        void removeNode(Node *prev, Node *current);

        Node * m_LastFoundNode = nullptr;
        Node * m_LastFoundPrevNode = nullptr;
        std::size_t m_LastFoundPadding = 0;
        size_t m_Used;
        size_t m_Peak;
        byte *m_StartPtr = nullptr;
        DoublyLinkedList<FreeHeader> m_FreeList;
        AVLTree<size_t, void *> m_SizeAVLTree;
        FreeListAllocatorPlacementPolicy m_Policy;
    };

public:
    FreeListAllocator(const size_t totalSize, const size_t alignment = 4);
    ~FreeListAllocator();
    void SetFreeListAllocatorPlacementPolicy(
        FreeListAllocatorPlacementPolicy Policy)
    {
        m_Policy = Policy;
    }
    virtual void *Allocate(const size_t allocSize,
                           const char *flie = nullptr,
                           size_t line);

    virtual void *Reallocate(void *ptr,
                             size_t size,
                             const char *flie = nullptr,
                             size_t line = 0) = 0;
    virtual void Free(void *ptr);

private:
    // FreeList 내 노드들은, 즉 노드포인터들은, 사실상 각 Block 의 시작 주소와 동일하다.
    // SinglyLinkedList<FreeHeader> m_FreeList;
    FreeListAllocatorPlacementPolicy m_Policy = FreeListAllocatorPlacementPolicy::FIND_BEST;
    size_t m_TotalMemorySize;
    size_t m_ScopeSize;
    size_t m_Alignment;
    std::vector<Scope *> m_Scopes;
};
} // namespace Hazel