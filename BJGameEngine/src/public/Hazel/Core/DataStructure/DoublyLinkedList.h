#pragma once

template<typename T>
class DoublyLinkedList
{
public:
	struct Node
	{
		T data;
		Node* next = nullptr;
		Node* prev = nullptr;
	};
	Node* m_Head;

public:
	void insert(Node* prevNode, Node*& newNode);
	void remove(Node* prevNode, Node*& deleteNode);
};

template<typename T>
inline void DoublyLinkedList<T>::insert(Node* prevNode,
	Node*& newNode)
{
	if (prevNode == nullptr)
	{
		// 만약 현재 newNode 가 첫번째 Node 라면
		newNode->next = m_Head;

		if (m_Head)
		{
			m_Head->prev = newNode;
		}

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

			newNode->prev = prevNode;
		}
		else
		{
			// newNode  ~ prev->next 연결
			newNode->next = prevNode->next;
			prevNode->next->prev = newNode;

			// prev ~ newNode 연결
			prevNode->next = newNode;
			newNode->prev = prevNode;
		}
	}
}

// deleteNode 를 List 상에서 지워주는 효과이다.
template<typename T>
inline void DoublyLinkedList<T>::remove(Node* prevNode,
	Node*& deleteNode)
{
	if (prevNode == nullptr)
	{
		// List 가 오직 하나의 원소만을 지니고 있다는 의미
		if (deleteNode->next == nullptr)
		{
			// 여기에서는 사실상 deleteNode == m_Head 였던 것
			m_Head = nullptr;
		}
		else
		{
			// List 가 여러개의 원소를 가지고 있다는 의미 
			// 그리고 list 상의 가장 첫번째 노드를 지우고 있는 경우에 해당한다.
			deleteNode->next->prev = nullptr;
			m_Head = deleteNode->next;
		}
	}
	else
	{
		// prevNode -> deleteNode -> Next
		// => (prevNode -> Next)
		Node* nextNode = deleteNode->next;
		prevNode->next = nextNode;

		if (nextNode)
		{
			nextNode->prev = prevNode;
		}
	}
}
