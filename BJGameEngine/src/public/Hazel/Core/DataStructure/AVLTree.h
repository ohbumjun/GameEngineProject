#pragma once
#include <cassert>
namespace Hazel
{

template <typename KEY, typename VALUE>
class AVLTreeNode
{
    template <typename KEY, typename VALUE>
    friend class AVLTree;
    template <typename KEY, typename VALUE>
    friend class AVLTreeIterator;

private:
    AVLTreeNode()
        : m_Left(nullptr), m_Right(nullptr), m_Parent(nullptr), m_Next(nullptr),
          m_Prev(nullptr)
    {
    }

    ~AVLTreeNode()
    {
    }

public:
    bool HasParent() const
    {
        return m_Parent != nullptr;
    }

    KEY GetParentKey() const
    {
        return m_Parent->first;
    }

    VALUE GetParentValue() const
    {
        return m_Parent->second;
    }

    bool HasLeft() const
    {
        return m_Left != nullptr;
    }

    KEY GetLeftKey() const
    {
        return m_Left->first;
    }

    VALUE GetLeftValue() const
    {
        return m_Left->second;
    }

    bool HasRight() const
    {
        return m_Right != nullptr;
    }

    KEY GetRightKey() const
    {
        return m_Right->first;
    }

    VALUE GetRightValue() const
    {
        return m_Right->second;
    }

    KEY first;
    VALUE second;

private:
    AVLTreeNode<KEY, VALUE> *m_Left = nullptr;
    AVLTreeNode<KEY, VALUE> *m_Right = nullptr;
    AVLTreeNode<KEY, VALUE> *m_Parent = nullptr;
    AVLTreeNode<KEY, VALUE> *m_Next = nullptr;
    AVLTreeNode<KEY, VALUE> *m_Prev = nullptr;

    // 같은 KEY 가 들어올 경우, 연결리스트 형태로 모아둔다.
    AVLTreeNode<KEY, VALUE> *m_NextInList = nullptr;
    AVLTreeNode<KEY, VALUE> *m_PrevInList = nullptr;
};

template <typename KEY, typename VALUE>
class AVLTreeIterator
{
    template <typename KEY, typename VALUE>
    friend class AVLTree;

public:
    AVLTreeIterator() : m_Node(nullptr)
    {
    }

    ~AVLTreeIterator()
    {
    }

private:
    AVLTreeNode<KEY, VALUE> *m_Node;

public:
    // iterator끼리 서로 가지고 있는 노드가 같을 경우 같다고 판단한다.
    bool operator==(const AVLTreeIterator<KEY, VALUE> &iter) const
    {
        return m_Node == iter.m_Node;
    }

    bool operator!=(const AVLTreeIterator<KEY, VALUE> &iter) const
    {
        return m_Node != iter.m_Node;
    }

    bool operator==(const AVLTreeNode<KEY, VALUE> *Node) const
    {
        return m_Node == Node;
    }

    bool operator!=(const AVLTreeNode<KEY, VALUE> *Node) const
    {
        return m_Node != Node;
    }

    void operator++()
    {
        m_Node = m_Node->m_Next;
    }

    void operator++(int)
    {
        m_Node = m_Node->m_Next;
    }

    void operator--()
    {
        m_Node = m_Node->m_Prev;
    }

    void operator--(int)
    {
        m_Node = m_Node->m_Prev;
    }

    AVLTreeNode<KEY, VALUE> *operator->()
    {
        return m_Node;
    }
};

// Key는 탐색을 하기 위한 타입이다.
// Value는 실제 저장하기 위한 데이터 타입이다.
// 그래서 탐색을 할때는 Key타입으로 한다.
// 만약 Key타입이 문자열이라면 문자열로 탐색을 할 수 있는 것이다.
template <typename KEY, typename VALUE>
class AVLTree
{
    typedef AVLTreeNode<KEY, VALUE> NODE;
    typedef AVLTreeIterator<KEY, VALUE> iterator;
    typedef AVLTreeNode<KEY, VALUE> *PNODE;

public:
    AVLTree()
    {
        m_Root = nullptr;
        m_Size = 0;

        m_Begin = new NODE;
        m_End = new NODE;

        m_Begin->m_Next = m_End;
        m_End->m_Prev = m_Begin;
    }

    ~AVLTree()
    {
        PNODE DeleteNode = m_Begin;

        while (DeleteNode)
        {
            PNODE Next = DeleteNode->m_Next;

            delete DeleteNode;

            DeleteNode = Next;
        }
    }


public:
    void Insert(const KEY &key, const VALUE &value)
    {
        // 처음 데이터를 추가할 경우
        if (!m_Root)
        {
            m_Root = new NODE;

            m_Root->first = key;
            m_Root->second = value;

            // begin -> root -> end 구조
            m_Begin->m_Next = m_Root;
            m_Root->m_Prev = m_Begin;

            m_Root->m_Next = m_End;
            m_End->m_Prev = m_Root;
        }

        else
        {
            insert(key, value, m_Root);
        }

        ++m_Size;
    }

    int Size() const
    {
        return m_Size;
    }

    bool Empty() const
    {
        return m_Size == 0;
    }

    iterator Begin() const
    {
        iterator iter;
        iter.m_Node = m_Begin->m_Next;
        return iter;
    }

    iterator End() const
    {
        iterator iter;
        iter.m_Node = m_End;
        return iter;
    }

    // 원래는 key 로만 찾았었는데,
    // 중복 key 를 허용하는 정책을 사용하기 때문에, value 도 같이 사용하여
    // 원하는 데이터를 찾는 로직을 적용한다.
    iterator Find(const KEY &key, const VALUE &value) const
    {
        return find(key, value, m_Root);
    }

    void Clear()
    {
        PNODE Node = m_Begin->m_Next;

        while (Node != m_End)
        {
            PNODE Next = Node->m_Next;

            delete Node;

            Node = Next;
        }

        m_Begin->m_Next = m_End;
        m_End->m_Prev = m_Begin;

        m_Size = 0;

        m_Root = nullptr;
    }

    /*
	이진트리는 3가지의 순회방법이 있다.
	전위순회, 중위순회, 후위순회
	전위순회 : Root -> Left -> Right
	중위순회 : Left -> Root -> Right
	후위순회 : Left -> Right -> Root
	*/
    void PreOrder(void (*pFunc)(const KEY &, const VALUE &))
    {
        PreOrder(pFunc, m_Root);
    }

    void InOrder(void (*pFunc)(const KEY &, const VALUE &))
    {
        InOrder(pFunc, m_Root);
    }

    void PostOrder(void (*pFunc)(const KEY &, const VALUE &))
    {
        PostOrder(pFunc, m_Root);
    }

    // 같은 key 를 제거하고, 그 연결리스트 상에서 만약 첫번째 노드가 되었다면
    // m_Right, m_Left 등의 정보들을 다시 세팅해줘야 한다.
    iterator Erase(const KEY &key, const VALUE &value)
    {
        iterator iter = Find(key, value);

        if (iter == End())
            return iter;

        return Erase(iter);
    }

    iterator Erase(const iterator &iter)
    {
        if (iter == End())
            return iter;

        else if (iter == m_Begin)
            return End();

        else if (iter.m_Node == nullptr)
            return End();

        PNODE returnNode = nullptr;

        bool firstNodeInList = isFirstInSameKeyList(iter.m_Node);

        // list 상에서 유일한 노드가 아니라면
        // 그냥 list 상에서 지워주고 끝낸다.
        if (isSingleNodeInList(iter.m_Node) == false)
        {
            PNODE deleteNextNodeInList = getNextNodeInList(iter.m_Node);
            returnNode = deleteNextNodeInList;
            eraseInSameKeyList(iter.m_Node);

            delete iter.m_Node;

            --m_Size;

            iterator result;
            result.m_Node = returnNode;
            return result;
        }

        // 리프노드일 경우 부모로부터 연결을 제거하고 노드를 제거해준다.
        if (!iter.m_Node->m_Left && !iter.m_Node->m_Right)
        {
            // 부모노드를 얻어온다.
            PNODE Parent = iter.m_Node->m_Parent;

            // 만약 부모노드가 없다면 현재 지우려는 노드는 루트노드라는 것이다.
            // 그런데 루트노드가 리프노드라는 말은 이 노드 1개만 남았다는
            // 것이다.
            if (!Parent)
            {
                // 여기로 들어왔다는 것은
                // 1) root node 이면서
                // 2) list 상에서 자기 혼자라는 의미이다.

                delete iter.m_Node;

                --m_Size;

                m_Root = nullptr;

                // 다 지웠으니 Begin과 End끼리 연결한다.
                m_Begin->m_Next = m_End;
                m_End->m_Prev = m_Begin;

                return End();
            }

            PNODE nextNodeInList = getNextNodeInList(iter.m_Node);

            // 지우려는 노드가 부모느드의 왼쪽 노드인지 오른쪽 노드인지를
            // 판단하여 부모와의 연결을 끊어준다.
            if (Parent->m_Left == iter.m_Node)
            {
                // Parent->m_Left = nullptr;
                Parent->m_Left = nextNodeInList;
            }
            else
            {
                // Parent->m_Right = nullptr;
                Parent->m_Right = nextNodeInList;
            }

            PNODE Prev = iter.m_Node->m_Prev;
            PNODE Next = iter.m_Node->m_Next;

            if (nextNodeInList)
            {
                Prev->m_Next = nextNodeInList;
                Next->m_Prev = nextNodeInList;
            }
            else
            {
                Prev->m_Next = Next;
                Next->m_Prev = Prev;
            }

            delete iter.m_Node;

            --m_Size;

            reBalance(Parent);


            // 지운 노드의 다음노드를 넣어서 iterator를 반환해준다.
            iterator result;
            result.m_Node = Next;

            return result;
        }

        // 만약 왼쪽 노드가 있을 경우 왼쪽 노드에서 가장 큰 노드를 찾아서
        // 지울 노드의 값을 변경해주고 찾아준 노드를 제거한다.
        if (iter.m_Node->m_Left)
        {
            // 왼쪽에 존재하는 노드들중 가장 큰 노드를 찾아온다.
            PNODE MaxNode = FindMax(iter.m_Node->m_Left);

            PNODE maxNextNodeInList = getNextNodeInList(MaxNode);

            // 여기로 들어온 다는 것은, iter.m_Node 가 List 상에서
            // 첫번째 노드라는 것이다.
            // 즉, 기존에 자기가 속한 list 상에서 위치 및 관계 정보를
            // updaet 할 필요는 없다.
            // 그저 새로 값이 세팅됨에 따라, 새로운 list 에
            // 해당 노드를 넣어주면 되는 것이다.

            assert(iter.m_Node->m_NextInList == nullptr);

            iter.m_Node->m_NextInList = maxNextNodeInList;

            if (maxNextNodeInList)
            {
                maxNextNodeInList->m_PrevInList = iter.m_Node;
            }

            // 찾아준 노드의 Key, Value 값으로 변경해준다.
            iter.m_Node->first = MaxNode->first;
            iter.m_Node->second = MaxNode->second;

            // 찾아준 노드를 제거해야하기 때문에 부모로부터 연결을 끊고
            // 제거해주도록 한다.
            // 단, 찾아준 노드가 왼쪽 자식노드가 있을수도 있으므로
            PNODE LeftChild = MaxNode->m_Left;
            PNODE Parent = MaxNode->m_Parent;

            if (Parent->m_Left == MaxNode)
                Parent->m_Left = LeftChild;

            else
                Parent->m_Right = LeftChild;

            // 왼쪽 자식노드가 있을 경우라면 부모로 MaxNode의 부모를
            // 지정해주도록 한다.
            if (LeftChild)
                LeftChild->m_Parent = Parent;

            PNODE Prev = MaxNode->m_Prev;
            PNODE Next = MaxNode->m_Next;

            Prev->m_Next = Next;
            Next->m_Prev = Prev;

            returnNode = Next;

            reBalance(Parent);

            delete MaxNode;

            --m_Size;

            iterator result;
            result.m_Node = returnNode;

            return result;
        }

        // 지울 노드의 오른쪽 노드만 존재할 경우 오른쪽 노드에서 가장 작은 노드를
        // 찾아온다.
        PNODE MinNode = FindMin(iter.m_Node->m_Right);

        PNODE minNextNodeInList = getNextNodeInList(MinNode);

        assert(iter.m_Node->m_NextInList == nullptr);

        iter.m_Node->m_NextInList = minNextNodeInList;

        if (minNextNodeInList)
        {
            minNextNodeInList->m_PrevInList = iter.m_Node;
        }

        // 찾아준 노드의 Key, Value 값으로 변경해준다.
        iter.m_Node->first = MinNode->first;
        iter.m_Node->second = MinNode->second;

        // 찾아준 노드를 제거해야하기 때문에 부모로부터 연결을 끊고
        // 제거해주도록 한다.
        // 단, 찾아준 노드가 오른쪽 자식노드가 있을수도 있으므로
        PNODE RightChild = MinNode->m_Right;
        PNODE Parent = MinNode->m_Parent;

        if (Parent->m_Left == MinNode)
            Parent->m_Left = RightChild;

        else
            Parent->m_Right = RightChild;

        // 오른쪽 자식노드가 있을 경우라면 부모로 MinNode의 부모를
        // 지정해주도록 한다.
        if (RightChild)
            RightChild->m_Parent = Parent;

        PNODE Prev = MinNode->m_Prev;
        PNODE Next = MinNode->m_Next;

        Prev->m_Next = Next;
        Next->m_Prev = Prev;

        returnNode = Next;

        reBalance(Parent);

        delete MinNode;

        --m_Size;


        iterator result;
        result.m_Node = returnNode;

        return result;
    }

    template <typename Function>
    void Find(Function fn)
    {
        PNODE findNode = m_Root;

        while (findNode)
        {
            int result = fn(findNode);

            // 원하는 값을 찾음
            if (result == 0)
                break;

            else if (result < 0)
                findNode = findNode->m_Left;
            else
                findNode = findNode->m_Right;
        }
    }

    PNODE FindMax(PNODE Node)
    {
        if (Node->m_Right)
            return FindMax(Node->m_Right);

        return Node;
    }

    PNODE FindMin(PNODE Node)
    {
        if (Node->m_Left)
            return FindMin(Node->m_Left);

        return Node;
    }

private:
    PNODE insert(const KEY &key, const VALUE &value, PNODE ParentNode)
    {
        // 같은 크기의 key 가 들어올 수도 있다. 이 경우는 어떻게 처리할 것인가
        if (ParentNode->first == key)
        {
            // 같은 key 들은 연결리스트 형태로 묶어둔다.
            PNODE lastNodeInList = ParentNode;

            while (lastNodeInList->m_NextInList)
                lastNodeInList = lastNodeInList->m_NextInList;

            PNODE NewNode = new NODE;

            NewNode->first = key;
            NewNode->second = value;
            NewNode->m_PrevInList = lastNodeInList;

            lastNodeInList->m_NextInList = NewNode;

            return NewNode;
        }

        // 기준노드보다 작다면 왼쪽이다.
        else if (ParentNode->first > key)
        {
            // 만약 기준노드의 왼쪽 자식노드가 있다면 그 왼쪽 자식노드를
            // 기준노드로 하여 다시 탐색을 하게 한다.
            if (ParentNode->m_Left)
                return insert(key, value, ParentNode->m_Left);

            // 더이상 왼쪽 자식노드가 없을 경우 이 위치에 새로 노드를 생성하여
            // 추가해주어야 한다.
            PNODE NewNode = new NODE;

            NewNode->first = key;
            NewNode->second = value;

            // 기준노드의 왼쪽 자식노드로 지정한다.
            ParentNode->m_Left = NewNode;
            NewNode->m_Parent = ParentNode;

            // 왼쪽으로 배치가 된다는것은 부모노드보다 작다는 것이다.
            // 그러므로 부모노드의 이전노드와 부모노드 사이에 새로 생성된
            // 노드를 리스트로 연결해주도록 한다.
            PNODE Prev = ParentNode->m_Prev;

            Prev->m_Next = NewNode;
            NewNode->m_Prev = Prev;

            NewNode->m_Next = ParentNode;
            ParentNode->m_Prev = NewNode;

            reBalance(NewNode);

            return NewNode;
        }

        // 여기로 내려오면 값이 크다는 것이므로 오른쪽으로 탐색을 해봐야 한다.
        // 만약 오른쪽 자식노드가 있을 경우 기준노드를 오른쪽 자식노드로 하여
        // 탐색하게 한다.
        if (ParentNode->m_Right)
            return insert(key, value, ParentNode->m_Right);

        // 더이상 오른쪽 자식노드가 없을 경우 이 위치에 새로 노드를 생성하여
        // 추가해주어야 한다.
        PNODE NewNode = new NODE;

        NewNode->first = key;
        NewNode->second = value;

        // 기준노드의 오른쪽 자식노드로 지정한다.
        ParentNode->m_Right = NewNode;
        NewNode->m_Parent = ParentNode;

        PNODE Next = ParentNode->m_Next;

        ParentNode->m_Next = NewNode;
        NewNode->m_Prev = ParentNode;

        NewNode->m_Next = Next;
        Next->m_Prev = NewNode;

        reBalance(NewNode);

        return NewNode;
    }

    iterator find(const KEY &key, const VALUE &value, PNODE Node) const
    {
        // 기준노드가 nullptr일 경우 더이상 탐색할 노드가 없으므로
        // end를 리턴한다. 못찾았다는 것이다.
        if (!Node)
            return End();

        if (Node->first == key)
        {
            // 같은 key 를 모아둔 연결리스트를 뒤져서 원하는 데이터를 찾는다.
            PNODE searchNode = Node;

            while (searchNode)
            {
                if (searchNode->second == value)
                {
                    iterator iter;
                    iter.m_Node = searchNode;
                    return iter;
                }

                searchNode = searchNode->m_NextInList;
            }

            // 여기에 걸리면 해당 key 로 된 value 가 존재하지 않는다는 것이다
            assert(false);
        }

        // 키를 비교하여 작으면 왼쪽, 크면 오른쪽으로 탐색해서 들어간다.
        if (Node->first > key)
        {
            return find(key, value, Node->m_Left);
        }

        return find(key, value, Node->m_Right);
    }

    void preOrder(void (*pFunc)(const KEY &, const VALUE &), PNODE Node)
    {
        if (!Node)
            return;

        pFunc(Node->first, Node->second);
        PreOrder(pFunc, Node->m_Left);
        PreOrder(pFunc, Node->m_Right);
    }

    void inOrder(void (*pFunc)(const KEY &, const VALUE &), PNODE Node)
    {
        if (!Node)
            return;

        InOrder(pFunc, Node->m_Left);
        pFunc(Node->first, Node->second);
        InOrder(pFunc, Node->m_Right);
    }

    void postOrder(void (*pFunc)(const KEY &, const VALUE &), PNODE Node)
    {
        if (!Node)
            return;

        PostOrder(pFunc, Node->m_Left);
        PostOrder(pFunc, Node->m_Right);
        pFunc(Node->first, Node->second);
    }

    PNODE rotationLeft(PNODE Node)
    {
        // 기준노드의 부모노드를 얻어온다.
        PNODE Parent = Node->m_Parent;

        // 기준노드의 오른쪽 자식노드를 얻어온다.
        PNODE RightChild = Node->m_Right;

        // 기준노드의 오른쪽 자식노드의 왼쪽 자식노드를 얻어온다.
        PNODE RightLeftChild = RightChild->m_Left;

        // 기준노드의 오른쪽 자식노드로 오른쪽자식노드의 왼쪽자식노드를 붙여준다.
        Node->m_Right = RightLeftChild;

        // 오른쪽 자식노드의 왼쪽 자식노드가 있을 경우 그 부모를 기준노드로 지정한다.
        if (RightLeftChild)
            RightLeftChild->m_Parent = Node;

        // 기준노드를 오른쪽 자식노드의 왼쪽 자식으로 붙여준다.
        RightChild->m_Left = Node;

        // 기준노드의 부모로 오른쪽 자식노드를 지정한다.
        Node->m_Parent = RightChild;

        // 오른쪽 자식노드의 부모노드를 기준노드의 부모노드로 지정한다.
        RightChild->m_Parent = Parent;

        // 기준노드의 부모노드가 있을경우 기준노드가 부모노드의 왼쪽자식인지 오른쪽 자식인지를
        // 판단하여 그 방향으로 기준노드의 오른쪽 자식노드를 붙여준다.
        if (Parent)
        {
            if (Parent->m_Left == Node)
                Parent->m_Left = RightChild;

            else
                Parent->m_Right = RightChild;
        }

        // 기준노드의 부모노드가 없다면 기준노드는 루트노드라는 의미이다.
        // 그러므로 기준노드의 자리에 RightChild가 오게되므로 루트노드를
        // RightChild로 교체해주어야 한다.
        else
        {
            m_Root = RightChild;
        }

        // 바뀐 기준노드를 반환한다.
        return RightChild;
    }

    PNODE rotationRight(PNODE Node)
    {
        // 기준노드의 부모노드를 얻어온다.
        PNODE Parent = Node->m_Parent;

        // 기준노드의 왼쪽 자식노드를 얻어온다.
        PNODE LeftChild = Node->m_Left;

        // 기준노드의 왼쪽 자식노드의 오른쪽 자식노드를 얻어온다.
        PNODE LeftRightChild = LeftChild->m_Right;

        // 기준노드의 왼쪽 자식노드로 왼쪽자식노드의 오른쪽자식노드를 붙여준다.
        Node->m_Left = LeftRightChild;

        // 왼쪽 자식노드의 오른쪽 자식노드가 있을 경우 그 부모를 기준노드로 지정한다.
        if (LeftRightChild)
            LeftRightChild->m_Parent = Node;

        // 왼쪽 자식노드의 오른쪽 자식으로 기준노드를 붙여준다.
        LeftChild->m_Right = Node;

        // 기준노드의 부모노드로 왼쪽 자식노드를 지정한다.
        Node->m_Parent = LeftChild;

        // 왼쪽자식노드의 부모로 기준노드의 부모노드를 지정한다.
        LeftChild->m_Parent = Parent;

        // 기준노드의 부모노드가 있을경우 기준노드가 부모노드의 왼쪽자식인지 오른쪽 자식인지를
        // 판단하여 그 방향으로 기준노드의 왼쪽 자식노드를 붙여준다.
        if (Parent)
        {
            if (Parent->m_Left == Node)
                Parent->m_Left = LeftChild;

            else
                Parent->m_Right = LeftChild;
        }

        // 기준노드의 부모노드가 없다면 기준노드는 루트노드라는 의미이다.
        // 그러므로 기준노드의 자리에 LeftChild가 오게되므로 루트노드를
        // LeftChild로 교체해주어야 한다.
        else
            m_Root = LeftChild;

        // 바뀐 기준노드를 반환한다.
        return LeftChild;
    }

    int getHeight(PNODE Node)
    {
        if (!Node)
            return 0;

        int Left = getHeight(Node->m_Left);
        int Right = getHeight(Node->m_Right);

        int Height = Left > Right ? Left : Right;

        // +1 ? 마지막 leaf 노드는 높이가 1 이 되게 한다.
        return Height + 1;
    }

    int balanceFactor(PNODE Node)
    {
        return getHeight(Node->m_Left) - getHeight(Node->m_Right);
    }

    bool isFirstInSameKeyList(PNODE Node)
    {
        return Node->m_PrevInList ? false : true;
    }

    bool isSingleNodeInList(PNODE Node)
    {
        return (Node->m_PrevInList || Node->m_NextInList) ? false : true;
    }

    void reBalance(PNODE Node)
    {
        if (!Node)
            return;

        // 왼쪽과 오른쪽의 높이차이를 구해준다.
        // 왼쪽 높이 - 오른쪽 높이
        int Factor = balanceFactor(Node);

        // 오른쪽으로 균형이 무너졌을 경우
        if (Factor <= -2)
        {
            int RightFactor = balanceFactor(Node->m_Right);

            // RR
            // 음수라면 오른쪽 오른쪽으로 균형이 무너졌을 경우이다.
            if (RightFactor <= 0)
            {
                // 기준노드를 중심으로 왼쪽으로 회전을 시켜준다.
                Node = rotationLeft(Node);
            }

            // RL => 1) R 회전 2) L 회전
            // 오른쪽 왼쪽으로 균형이 무너졌을 경우이다.
            else
            {
                // 오른쪽 자식노드를 중심으로 오른쪽 회전을 한다.
                rotationRight(Node->m_Right);

                // 기준노들를 중심으로 왼쪽 회전을 시켜준다.
                Node = rotationLeft(Node);
            }
        }

        // 왼쪽으로 균형이 무너졌을 경우
        else if (Factor >= 2)
        {
            int LeftFactor = balanceFactor(Node->m_Left);

            // 왼쪽 왼쪽으로 균형이 무너졌을 경우
            if (LeftFactor >= 0)
            {
                // 기준노드를 중심으로 오른쪽으로 회전한다.
                Node = rotationRight(Node);
            }

            // 왼쪽 오른쪽으로 균형이 무너졌을 경우
            else
            {
                // 기준노드의 왼쪽 자식노드를 중심으로 왼쪽으로 회전한다.
                rotationLeft(Node->m_Left);

                // 기준노드를 중심으로 오른쪽으로 회전한다.
                Node = rotationRight(Node);
            }
        }

        reBalance(Node->m_Parent);
    }

    PNODE getNextNodeInList(PNODE Node)
    {
        return Node->m_NextInList;
    }

    // 같은 Key 를 가진  List 상에서 해당 Node 정보를 지워주는 함수
    void eraseInSameKeyList(PNODE Node)
    {
        bool isFirstNodeInList = Node->m_PrevInList ? false : true;

        PNODE prevNodeInList = Node->m_PrevInList;
        PNODE nextNodeInList = Node->m_NextInList;

        // Node 가 현재 List 상에서 첫번째 Node 일 수 있다.
        if (prevNodeInList)
        {
            prevNodeInList->m_NextInList = nextNodeInList;
        }

        // List 상에서 마지막 노드일 수도 있다.
        if (nextNodeInList)
        {
            nextNodeInList->m_PrevInList = prevNodeInList;
        }

        // 그 다음 m_Next, m_Prev 등 정보도 다시 세팅해줄 것이다.
        if (isFirstNodeInList)
        {
            // nextNode 가 이제 list 상의 첫번째 노드가 되는 것이다.
            PNODE nextNode = Node->m_Next;
            PNODE prevNode = Node->m_Prev;
            PNODE leftNode = Node->m_Left;
            PNODE rightNode = Node->m_Right;
            PNODE parentNode = Node->m_Parent;

            if (!parentNode)
            {
                // Node 가 Root Node 였다는 의미이다.
                m_Root = nextNodeInList;
                m_Begin->m_Next = m_End;
                m_End->m_Prev = m_Begin;
            }
            else
            {
                // 부모 노드가 있다면
                if (parentNode->m_Left == Node)
                    parentNode->m_Left = nextNodeInList;
                else if (parentNode->m_Right == Node)
                    parentNode->m_Right = nextNodeInList;
                else
                {
                    // 여기에 걸리면 무조건 Crash
                    assert(false);
                }

                nextNodeInList->m_Parent = parentNode;
            }

            prevNode->m_Next = nextNodeInList;
            nextNodeInList->m_Prev = prevNode;

            nextNode->m_Prev = nextNodeInList;
            nextNodeInList->m_Next = nextNode;

            nextNodeInList->m_Left = leftNode;
            if (leftNode)
            {
                leftNode->m_Parent = nextNodeInList;
            }

            nextNodeInList->m_Right = rightNode;
            if (rightNode)
            {
                rightNode->m_Parent = nextNodeInList;
            }
        }
    }

private:
    PNODE m_Root;
    PNODE m_Begin;
    PNODE m_End;
    int m_Size;
};
} // namespace Hazel