#pragma once

namespace Hazel
{

template <typename T>
class SinglyLinkedList
{
public:
    struct Node
    {
        T data;
        Node *next;
    };
    Node *m_Head;

public:
    void insert(Node *prevNode, Node *&newNode);
    void remove(Node *prevNode, Node *&deleteNode);
};

template <typename T>
inline void SinglyLinkedList<T>::insert(Node *prevNode, Node *&newNode)
{
    if (prevNode == nullptr)
    {
        // 만약 현재 newNode 가 첫번째 Node 라면
        newNode->next = m_Head;

        // newNode (= new Head) -> old Head
        // inserting to 'front'
        m_Head = newNode;
    }
    else
    {
        // prevNode 다음에 newNode 를 넣을 것이다.
        if (prevNode->next == nullptr)
        {
            prevNode->next = newNode;
            newNode->next = nullptr;
        }
        else
        {
            newNode->next = prevNode->next;
            prevNode->next = newNode;
        }
    }
}

// deleteNode 를 List 상에서 지워주는 효과이다.
template <typename T>
inline void SinglyLinkedList<T>::remove(Node *prevNode, Node *&deleteNode)
{
    if (prevNode == nullptr)
    {
        // List 가 오직 하나의 원소만을 지니고 있다는 의미
        if (deleteNode->next == nullptr)
        {
            m_Head = nullptr;
        }
        else
        {
            // List 가 여러개의 원소를 가지고 있다는 의미
            m_Head = deleteNode->next;
        }
    }
    else
    {
        // prevNode -> deleteNode -> Next
        // => (prevNode -> Next)
        prevNode->next = deleteNode->next;
    }
}
} // namespace Hazel