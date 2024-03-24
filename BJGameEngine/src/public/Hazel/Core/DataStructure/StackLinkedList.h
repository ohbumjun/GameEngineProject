#pragma once
namespace Hazel
{

template <typename T>
class StackLinkedList
{
    friend class CPoolAllocator;

public:
    struct Node
    {
        T data;
        Node *Next = nullptr;
    };

private:
    int m_TestSize;
    Node *m_Head;

public:
    StackLinkedList() : m_Head(nullptr)
    {
    }
    StackLinkedList(StackLinkedList &List) = delete;
    void push(Node *newNode);
    Node *pop();
};

template <typename T>
inline void StackLinkedList<T>::push(Node *newNode)
{
    if (!newNode)
        return;

    newNode->Next = m_Head;
    m_Head = newNode;

    m_TestSize += 1;
};

template <typename T>
inline typename StackLinkedList<T>::Node *StackLinkedList<T>::pop()
{
    m_TestSize -= 1;

    Node *PopNode = m_Head;

    if (m_Head)
        m_Head = m_Head->Next;

    return PopNode;
};
} // namespace Hazel