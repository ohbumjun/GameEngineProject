#pragma once

#include "GCInfo.h"
#include "Hazel/Core/DataStructure/DoublyLinkedList.h"
#include "Hazel/Core/DataStructure/AVLTree.h"

class GCObject;
class MyPoolAllocator;
class MemoryPoolManager;

class GCAllocator
{
    struct GCFreeHeader
    {
        // 해당 block의 크기가 몇인지
        // Find first, Find Best 등의 알고리즘을 수행할 때 해당 blockSize 로
        // 비교를 수행하게 된다.
        size_t blockSize;
    };
    typedef unsigned char byte;
    // typedef SinglyLinkedList<FreeHeader>::Node Node;
    typedef DoublyLinkedList<GCFreeHeader>::Node Node;

public :
    GCAllocator(size_t totalSize);
    ~GCAllocator();
	template<typename T>
	GCObject* Allocate(bool isRoot = false);
	void Free(GCObject* ptr);
    virtual void Init();
    void Reset();

private:
    void coalescene(Node* prevBlock, Node* freeBlock);
    void find(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode);
    void findSpeed(const size_t allocSize, const size_t alignment, size_t& padding, Node*& prevNode, Node*& foundNode);
    void insertNode(Node* prev, Node* current);
    void removeNode(Node* prev, Node* current);

    template<typename T>
    void allocate();
    void free(void* dataPtr);

    // GCFreeHeader* 형태의 노드들로 이루어진 List 가 된다.
    // 아래 2개는 실제 T Type 의 객체를 할당하기 위한 Allocate 수단이 된다.
	DoublyLinkedList<GCFreeHeader> m_FreeList;
	AVLTree<size_t, void*> m_SizeAVLTree;

    // 아래의 PoolAllocator 는 GCObject 를 할당하기 위한 Class 이다.
    // 즉, 하나의 Object 를 할당할 때, 그에 맞는 GCObject 도 할당할 것이다.
    MyPoolAllocator* m_GCObjectPool;
    MemoryPoolManager* m_memoryPoolManager;

    // 차후에는 Area 라고 하여 여러개의 Pool ?
    // 을 가지고 있을 수 있게 할 것이다.
    size_t m_TotalSize;
    byte* m_StartPtr = nullptr;
    size_t m_Used;
    size_t m_Peak;
    size_t m_Alignment;
};

