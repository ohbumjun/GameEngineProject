#include "Hazel/Core/Allocation/Allocator/FreeListAllocator.h"
#include "Hazel/Utils/EngineUtil.h"
#include "hzpch.h"
#include <limits> /* limits_max */

#define INF int(1e9)
namespace Hazel
{

#pragma region Scope

void FreeListAllocator::Scope::coalescene(Node *prevBlock, Node *freeBlock)
{
    if (freeBlock->next != nullptr &&
        (std::size_t)freeBlock + freeBlock->data.blockSize ==
            (size_t)freeBlock->next)
    {
        if (m_Policy == FreeListAllocatorPlacementPolicy::FIND_SPEED)
        {
            // freeBlock 정보를 update 해야 한다.
            // 따라서 기존의 freeBlock Block 정보를 Tree 에서 지워준다.
            m_SizeAVLTree.Erase(freeBlock->data.blockSize, freeBlock);

            // freeBlock->next 에 해당하는 것을 지운다.
            m_SizeAVLTree.Erase(freeBlock->next->data.blockSize,
                                freeBlock->next);
        }

        freeBlock->data.blockSize += freeBlock->next->data.blockSize;
        m_FreeList.remove(freeBlock, freeBlock->next);

        if (m_Policy == FreeListAllocatorPlacementPolicy::FIND_SPEED)
        {
            // update 된 새로운 freeBlock 을 넣어준다.
            m_SizeAVLTree.Insert(freeBlock->data.blockSize, freeBlock);
        }
    }

    if (prevBlock != nullptr &&
        (size_t)prevBlock + prevBlock->data.blockSize == (std::size_t)freeBlock)
    {
        if (m_Policy == FreeListAllocatorPlacementPolicy::FIND_SPEED)
        {
            // prevBlock 정보를 update 해야 한다.
            // 따라서 기존의 prev Block 정보를 Tree 에서 지워준다.
            m_SizeAVLTree.Erase(prevBlock->data.blockSize, prevBlock);
        }

        prevBlock->data.blockSize += freeBlock->data.blockSize;

        m_FreeList.remove(prevBlock, freeBlock);

        if (m_Policy == FreeListAllocatorPlacementPolicy::FIND_SPEED)
        {
            // update 된 새로운 prevBlock 정보를 등록해준다.
            m_SizeAVLTree.Insert(prevBlock->data.blockSize, prevBlock);

            // 한편 freeBlock 정보는 지워준다.
            m_SizeAVLTree.Erase(freeBlock->data.blockSize, freeBlock);
        }
    }
}

FreeListAllocator::Scope::Scope(size_t totalSize,
                                FreeListAllocatorPlacementPolicy policy)
{
    m_Policy = policy;

    m_StartPtr = new byte[totalSize];

    m_Used = 0;
    m_Peak = 0;

    Node *firstNode = (Node *)m_StartPtr;
    firstNode->data.blockSize = totalSize;
    firstNode->next = nullptr;
    firstNode->prev = nullptr;

    m_FreeList.m_Head = nullptr;
    m_FreeList.insert(nullptr, firstNode);

    m_SizeAVLTree.Clear();

    if (m_Policy == FreeListAllocatorPlacementPolicy::FIND_SPEED)
    {
        m_SizeAVLTree.Insert(totalSize, firstNode);
    }
}

FreeListAllocator::Scope::~Scope()
{
	delete m_StartPtr;
    m_StartPtr = nullptr;
}

bool FreeListAllocator::Scope::HasEnoughSpace(const size_t allocSize,
                                              const size_t alignment)
{
    const size_t allocHeaderSize = sizeof(FreeListAllocator::AllocationHeader);
    const size_t freeHeaderSize = sizeof(FreeListAllocator::FreeHeader);

    size_t nodeSize = sizeof(Node);
    // assert(allocSize >= nodeSize);
    assert(allocSize >= 8);

    // Free Memory Block 을 돌면서
    // 최적의 Free Memory Block 을 찾는다.
    Node *affectedNode = nullptr, *prevNode = nullptr;

    find(allocSize,
         alignment,
         m_LastFoundPadding,
         m_LastFoundPrevNode,
         affectedNode);

    m_LastFoundNode = affectedNode;

    return m_LastFoundNode != nullptr;
}

bool FreeListAllocator::Scope::Contain(void *dataAddress, size_t scopeSize)
{ 
    // m_Memory[0] : 해당 Area 의 메모리 시작 위치
    const bool s = &m_StartPtr[0] <= dataAddress;

    // m_Memory[m_Chunk] : 해당 Area 의 메모리 끝 위치 (혹은 다음에 할당할 Block 위치)
    const bool l = dataAddress <= &m_StartPtr[scopeSize];

    return s && l;
}

void *FreeListAllocator::Scope::Alloc(const size_t allocSize)
{
    const size_t allocHeaderSize = sizeof(FreeListAllocator::AllocationHeader);
    const size_t freeHeaderSize = sizeof(FreeListAllocator::FreeHeader);
    size_t nodeSize = sizeof(Node);

    const size_t alignmentPadding = m_LastFoundPadding - allocHeaderSize;
    const size_t requiredSize = allocSize + m_LastFoundPadding;

    const size_t rest = m_LastFoundNode->data.blockSize - requiredSize;

    // split block into data block, and a free block of size 'rest'
    Node *newFreeNode = (Node *)((std::size_t)m_LastFoundNode + requiredSize);

    if (rest > 0)
    {
        newFreeNode->data.blockSize = rest;
        newFreeNode->next = nullptr;

        // m_FreeList.insert(affectedNode, newFreeNode);
        insertNode(m_LastFoundNode, newFreeNode);
    }

    // m_FreeList.remove(prevNode, affectedNode);
    removeNode(m_LastFoundPrevNode, m_LastFoundNode);

    // set up date block
    const std::size_t headerAddress =
        (size_t)m_LastFoundNode + alignmentPadding;
    const std::size_t dataAdderess = headerAddress + allocHeaderSize;

    FreeListAllocator::AllocationHeader *HeaderPtr =
        (FreeListAllocator::AllocationHeader *)headerAddress;

    HeaderPtr->blockSize = requiredSize;

    // headerAddress 로부터, padding 만큼 이전에 가면
    // 가장 마지막에 할당된 datablock 의 끝 위치가 나온다는 의미
    HeaderPtr->padding = (char)alignmentPadding;

    m_Used += requiredSize;
    m_Peak = m_Used;

    return (void *)dataAdderess;
}

void FreeListAllocator::Scope::Free(void *ptr)
{
    // Insert It in a sorted position by the address number
    // - Data Address
    const std::size_t currentAddresss = (std::size_t)ptr;
    const std::size_t headerAddress =
        currentAddresss - sizeof(FreeListAllocator::AllocationHeader);

    const FreeListAllocator::AllocationHeader *allocHeader(
        (FreeListAllocator::AllocationHeader *)headerAddress);

    /*
	Node* freeNode = (Node*)(headerAddress);
	freeNode->data.blockSize = allocHeader->blockSize + allocHeader->padding;
	*/
    Node *freeNode =
        (Node *)((std::size_t)headerAddress - allocHeader->padding);
    freeNode->data.blockSize = allocHeader->blockSize;

    freeNode->next = nullptr;

    Node *iter = m_FreeList.m_Head;
    Node *iterPrev = nullptr;

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

    // Tree 정보에 새로 생성한 freeNode 정보를 넣어준다.
    if (m_Policy == FreeListAllocatorPlacementPolicy::FIND_SPEED)
    {
        m_SizeAVLTree.Insert(freeNode->data.blockSize, freeNode);
    }

    // Merge Contiguous Nodes
    coalescene(iterPrev, freeNode);
}

void FreeListAllocator::Scope::find(const size_t allocSize,
                             const size_t alignment,
                             size_t &padding,
                             Node *&prevNode,
                             Node *&foundNode)
{
    switch (m_Policy)
    {
    case FreeListAllocatorPlacementPolicy::FIND_FIRST:
    {
        findFirst(allocSize, alignment, padding, prevNode, foundNode);
    }
    break;
    case FreeListAllocatorPlacementPolicy::FIND_BEST:
    {
        findBest(allocSize, alignment, padding, prevNode, foundNode);
    }
    break;
    case FreeListAllocatorPlacementPolicy::FIND_SPEED:
    {
        findSpeed(allocSize, alignment, padding, prevNode, foundNode);
    }
    break;
    }
}

void FreeListAllocator::Scope::findBest(const size_t allocSize,
                                 const size_t alignment,
                                 size_t &padding,
                                 Node *&prevNode,
                                 Node *&foundNode)
{
    size_t smallestDiff = (size_t)INF;

    Node *bestBlock = nullptr;
    Node *prevBestBlock = nullptr;
    Node *iter = m_FreeList.m_Head;
    Node *iterPrev = nullptr;

    while (iter != nullptr)
    {
        padding = Utils::EngineUtil::CalculatePaddingWithHeader(
            (size_t)iter,
            alignment,
            sizeof(FreeListAllocator::AllocationHeader));

        const size_t requiredSpace = allocSize + padding;

        if (iter->data.blockSize >= requiredSpace &&
            (iter->data.blockSize - requiredSpace) < smallestDiff)
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

void FreeListAllocator::Scope::findFirst(const size_t allocSize,
                                  const size_t alignment,
                                  size_t &padding,
                                  Node *&prevNode,
                                  Node *&foundNode)
{
    Node *iter = m_FreeList.m_Head;
    Node *iterPrev = nullptr;

    while (iter != nullptr)
    {
        padding = Utils::EngineUtil::CalculatePaddingWithHeader(
            (size_t)iter,
            alignment,
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

void FreeListAllocator::Scope::findSpeed(const size_t allocSize,
                                  const size_t alignment,
                                  size_t &padding,
                                  Node *&prevNode,
                                  Node *&foundNode)
{
    assert(m_SizeAVLTree.Size() > 0);

    Node *findNode = nullptr;

    size_t minValue = static_cast<size_t>(-1);

    m_SizeAVLTree.Find([&findNode, allocSize, &minValue, &padding, alignment](
                           const AVLTreeNode<size_t, void *> *node) {
        // node->second 는 AVL Tree 상의 node
        // 반면, Node 는, DoublyLinkedList 상의 Node
        Node *iter = reinterpret_cast<Node *>(node->second);

        padding = Utils::EngineUtil::CalculatePaddingWithHeader(
            (size_t)iter,
            alignment,
            sizeof(FreeListAllocator::AllocationHeader));

        size_t requireSize = padding + allocSize;

        // 만족하는 가장 최적의 크기를 찾는다.
        if (node->first >= requireSize)
        {
            if (node->first < minValue)
            {
                minValue = node->first;
                findNode = iter;
                // 더 작은 최적의 메모리를 찾아 들어간다
                return -1;
            }
        }
        else
        {
            // node->first < requireSize
            // 더 큰 블록을 찾아들어간다.
            return 1;
        }

        return 0;
    });


    assert(findNode); // 할당하지 않은 메모리를 해제한 것일 수 있다.

    prevNode = findNode->prev;
    foundNode = findNode;
}

// freeList insert, remove 부분들을 아래 함수들로 대체해야 한다.
void FreeListAllocator::Scope::insertNode(Node *prev, Node *current)
{
    m_FreeList.insert(prev, current);

    if (m_Policy == FreeListAllocatorPlacementPolicy::FIND_SPEED)
    {
        m_SizeAVLTree.Insert(current->data.blockSize, current);
    }
}

void FreeListAllocator::Scope::removeNode(Node *prev, Node *current)
{
    m_FreeList.remove(prev, current);

    if (m_Policy == FreeListAllocatorPlacementPolicy::FIND_SPEED)
    {
        m_SizeAVLTree.Erase(current->data.blockSize, current);
    }
}
#pragma endregion

FreeListAllocator::FreeListAllocator(const size_t totalSize,
                                     const size_t alignment)
    : m_ScopeSize(totalSize), m_Alignment(alignment)
{
    Scope *newScope = new Scope(m_ScopeSize, m_Policy);
    m_Scopes.push_back(newScope);

    m_TotalMemorySize += m_ScopeSize;
}

FreeListAllocator::~FreeListAllocator()
{
for (Scope *scope : m_Scopes)
	{
		delete scope;
	}
	m_Scopes.clear();
}

void *FreeListAllocator::Allocate(const size_t allocSize,
                                  const char *flie,
                                  size_t line)
{   
    assert(allocSize != 0, "allocSize must be greater than 0");

    for (Scope* scope : m_Scopes)
    {
        if (scope->HasEnoughSpace(allocSize, m_Alignment))
        {
            return scope->Alloc(allocSize);
        }
    }

    m_TotalMemorySize += m_ScopeSize;

    Scope *newScope = new Scope(m_ScopeSize, m_Policy);
    m_Scopes.push_back(newScope);

    return newScope->Alloc(allocSize);
}

void *FreeListAllocator::Reallocate(void *ptr,
                                    size_t size,
                                    const char *flie,
                                    size_t line)
{
    if (size > m_ScopeSize)
        THROW("Size is larger than Chunk");

    // 우선 메모리 할당을 한다.
    void *r = Allocate(size, flie, line);

    // ptr 에 있는 메모리를 r 로 복사한다.
    memcpy(r, ptr, size);

    // ptr 은 해제 한다.
    Free(ptr);

    return r;
}

void FreeListAllocator::Free(void *dataAddress)
{
    for (size_t i = 0; i < m_Scopes.size(); ++i)
    {
        if (m_Scopes[i]->Contain(dataAddress, m_ScopeSize))
        {
            m_Scopes[i]->Free(dataAddress);
        }
    }
}

} // namespace Hazel