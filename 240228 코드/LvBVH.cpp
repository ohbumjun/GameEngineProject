#include "engine/LvBVH.h"
#include "LvPrecompiled.h"

#include "system/LvLog.h"

LV_NS_ENGINE_BEGIN

LvIntersection LvIntersection::operator()(
    const LvBoxBound &bound,
    const LvRay &ray,
    uint32 id,
    DetectingIntersect detector) const noexcept
{
    return detector(bound, ray, id);
}

LvIntersection LvIntersection::BasicDetectingIntersect(const LvBoxBound &bound,
                                                       const LvRay &ray,
                                                       uint32 id)
{
    float tnear, tfar;
    if (bound.Intersect(ray, tnear, tfar))
        return LvIntersection(tnear, bound, id);
    else
        return LvIntersection();
}

LvBVH::LvBVH() : _tree(512 * sizeof(LvBVH::Pair))
{
}

LvBVH::LvBVH(LvBoxBound *datas, const uint32 size, LvList<uint32> &outIndices)
    : _tree(LV_MAX(512, size) * sizeof(Pair))
{
    Rebuild(datas, size, outIndices);
}

LvBVH::~LvBVH()
{
    _tree.Clear();
}

uint32 LvBVH::Insert(const LvBoxBound &data) noexcept
{
    LvVec3f center = data.GetCenter();
    Pair npair(data);

    if (_leafCount == 0)
        return _tree.Add(npair);

    Pair target = _tree.GetChilds()[0];
    while (true)
    {
        uint32 parent = target.index;
        float minDist = LV_FLT_MAX;
        for (const Pair &pair : _tree.GetChildsAt(parent))
        {
            float distance = LvVec3f::Length(center - pair.b.GetCenter());
            if (minDist > distance)
            {
                minDist = distance;
                target = pair;
            }
        }

        uint32 child = target.index;
        if (!_tree.HasChildAt(child)) //leaf
        {
            //if   : leaf's parent has enough space, just add
            //else : leaf's parent has full leaf, make new branch and add child
            if (_tree.DegreeAt(child) < _leafSize - 1)
            {
                uint32 index = _tree.AddAt(npair, parent);
                _tree[index].index = index;
                return index;
            }
            else
            {
                Pair origin = _tree[child];

                uint32 first = _tree.AddAt(npair, parent);
                uint32 second = _tree.AddAt(npair, first);
                //To keep index unchanged
                _tree.RemoveAt(child);
                _tree.AddAt(origin, first);

                _tree[first].index = first;
                _tree[first].b.ExpandToInclude(origin.b);
                _tree[second].index = second;
                return second;
            }
        }
        else //branch
        {
            _tree[child].b.ExpandToInclude(data);
        }

        target = _tree[child];
    }
}

bool LvBVH::Remove(const LvBoxBound &data) noexcept
{
    Pair pair(data);
    uint32 id = _tree.Find(pair);
    if (id != uint32(-1))
        return Remove(id);
    return false;
}

bool LvBVH::Remove(uint32 id) noexcept
{
    if (_tree.HasChildAt(id))
        return false;

    uint32 p = _tree.GetParentIndexAt(id);
    _tree.RemoveAt(id);
    update(p);
    return true;
}

void LvBVH::Rebuild(LvBoxBound *datas,
                    const uint32 size,
                    LvList<uint32> &outIndices) noexcept
{
    if (datas == nullptr || size == 0)
        return;

    _tree.Clear();
    outIndices.Clear();
    outIndices.Resize(size);

    BuildNode root{_tree.GetRoot(), 0, size};
    LvStack<BuildNode> nodes;
    LvStack<LvPair<uint32, uint32>> swapped;
    nodes.Push(root);

    while (!nodes.IsEmpty())
    {
        const BuildNode &node = nodes.Pop();

        uint32 parent = node.parent;
        uint32 start = node.start;
        uint32 end = node.end;
        uint32 count = end - start;

        if (count < _leafSize)
        {
            for (uint32 i = start; i < end; ++i)
            {
                Pair pair(datas[i]);
                outIndices[i] = _tree.AddAt(pair, parent);
                _tree[outIndices[i]].index = outIndices[i];
                ++_leafCount;
            }
            continue;
        }

        LvBoxBound nodeBox = datas[start];
        LvBoxBound nodeCenter; //(nodeBox.GetCenter());
        nodeCenter.minp = nodeCenter.maxp = nodeBox.GetCenter();
        for (uint32 i = start + 1; i < end; ++i)
        {
            nodeBox.ExpandToInclude(datas[i]);
            nodeCenter.ExpandToInclude(datas[i].GetCenter());
        }

        uint32 index = _tree.AddAt(nodeBox, parent);
        _tree[index].index = index;

        uint32 splitDim = nodeCenter.MaxDimension();
        float split = (nodeBox.minp[splitDim] + nodeBox.maxp[splitDim]) / 2;

        uint32 mid = start;
        for (uint32 i = start + 1; i < end; ++i)
        {
            if (datas[i].GetCenter()[splitDim] < split)
            {
                LV_SWAP(datas[i], datas[mid], LvBoxBound);
                swapped.Push(LvPair<uint32, uint32>(i, mid));
                ++mid;
            }
        }

        if (mid == start || mid == end)
            mid = (start + end) / 2;

        nodes.Push(BuildNode{index, start, mid});
        nodes.Push(BuildNode{index, mid, end});
    }

    while (!swapped.IsEmpty())
    {
        LvPair<uint32, uint32> swappair = swapped.Pop();
        LV_SWAP(outIndices[swappair.first],
                outIndices[swappair.second],
                uint32);
    }
}

LvIntersection LvBVH::Intersect(const LvRay &ray) noexcept
{
    LvIntersection intersection;
    LvStack<uint32> nodes;

    const LvList<Pair> &pairs = _tree.GetChilds();

    if (pairs.Count() < 1)
    {
        return intersection;
    }

    for (const Pair &pair : pairs)
        nodes.Push(pair.index);

    while (!nodes.IsEmpty())
    {
        uint32 index = nodes.Pop();

        if (!_tree.HasChildAt(index)) //leaf
        {
            LvIntersection nIntersection =
                intersection(_tree[index].b, ray, index, _detector);
            if (nIntersection)
                intersection =
                    LvIntersection::Closest(intersection, nIntersection);
        }
        else //branch
        {
            for (const Pair &npair : _tree.GetChildsAt(index))
            {
                float nears, fars;
                if (npair.b.Intersect(ray, nears, fars))
                    nodes.Push(npair.index);
            }
        }
    }
    return intersection;
}

LvIntersection LvBVH::Intersect(const LvRay &ray,
                                RankingIntersect ranker) noexcept
{
    LvIntersection intersection;
    LvStack<uint32> nodes;

    const LvList<Pair> &pairs = _tree.GetChilds();

    if (pairs.Count() < 1)
    {
        LV_LOG(crash, "No elements in tree");
        return intersection;
    }

    for (const Pair &pair : pairs)
        nodes.Push(pair.index);

    while (!nodes.IsEmpty())
    {
        uint32 index = nodes.Pop();

        if (!_tree.HasChildAt(index)) //leaf
        {
            LvIntersection nIntersection =
                intersection(_tree[index].b, ray, index, _detector);
            if (nIntersection)
                intersection = ranker(intersection, nIntersection);
        }
        else //branch
        {
            for (const Pair &npair : _tree.GetChildsAt(index))
            {
                float nears, fars;
                if (npair.b.Intersect(ray, nears, fars))
                    nodes.Push(npair.index);
            }
        }
    }
    return intersection;
}

void LvBVH::update(const uint32 id) noexcept
{
    uint32 parent = _tree.GetParentIndexAt(id);
    uint32 root = _tree.GetRoot();

    while (parent != root)
    {
        LvBoxBound &bound = _tree[id].b;
        bound.Clear();
        for (const Pair &pair : _tree.GetChildsAt(id))
            bound.ExpandToInclude(pair.b);

        parent = _tree.GetParentIndexAt(parent);
    }
}

const float LvDynamicAABBTree::aabbExtension = 0.01f;
const int LvDynamicAABBTree::nodeNull = -1;
LvDynamicAABBTree::LvDynamicAABBTree()
{
    _root = nodeNull;

    _nodeCapacity = 16;
    _nodeCount = 0;
    _nodes = new TreeNode[_nodeCapacity];

    // Build a linked list for the free list
    for (int i = 0; i < _nodeCapacity - 1; ++i)
    {
        _nodes[i].next = i + 1;
        _nodes[i].height = -1;
    }
    _nodes[_nodeCapacity - 1].next = nodeNull;
    _nodes[_nodeCapacity - 1].height = -1;
    _freeList = 0;

    _insertionCount = 0;

    _stackCapacity = 256;
    _stack = static_cast<int *>(lv_malloc(sizeof(int) * _stackCapacity));
}

LvDynamicAABBTree::LvDynamicAABBTree(int nodeCapacity)
{
    _root = nodeNull;

    _nodeCapacity = nodeCapacity;
    _nodeCount = 0;
    _nodes = new TreeNode[_nodeCapacity];

    // Build a linked list for the free list
    for (int i = 0; i < _nodeCapacity - 1; ++i)
    {
        _nodes[i].next = i + 1;
        _nodes[i].height = -1;
    }
    _nodes[_nodeCapacity - 1].next = nodeNull;
    _nodes[_nodeCapacity - 1].height = -1;
    _freeList = 0;

    _insertionCount = 0;

    _stackCapacity = 256;
    _stack = static_cast<int *>(lv_malloc(sizeof(int) * _stackCapacity));
}

LvDynamicAABBTree::~LvDynamicAABBTree()
{
    lv_free(_stack);
    _stack = nullptr;

    delete[] _nodes;
    _nodes = nullptr;
}

int LvDynamicAABBTree::CreateProxy(const LvBoxBound &aabb, void *userData)
{
    int proxyId = allocateNode();

    // Fatten the aabb.
    LvVec3f r(aabbExtension);
    _nodes[proxyId].aabb.minp = aabb.minp - r;
    _nodes[proxyId].aabb.maxp = aabb.maxp + r;
    _nodes[proxyId].userData = userData;
    _nodes[proxyId].height = 0;

    insertLeaf(proxyId);

    return proxyId;
}

void LvDynamicAABBTree::DestroyProxy(int proxyId)
{
    LV_CHECK(0 <= proxyId && proxyId < _nodeCapacity, "Invalid Proxy Id");
    LV_CHECK(_nodes[proxyId].isLeaf(), "Invalid Proxy Id");

    removeLeaf(proxyId);
    freeNode(proxyId);
}

bool LvDynamicAABBTree::UpdateProxy(int proxyId,
                                    const Engine::LvBoxBound &aabb,
                                    const LvVec3f displacement)
{
    LV_CHECK(0 <= proxyId && proxyId < _nodeCapacity, "Invalid Proxy Id");
    LV_CHECK(_nodes[proxyId].isLeaf(), "Invalid Proxy Id");

    if (_nodes[proxyId].aabb.Contains(aabb))
        return false;

    removeLeaf(proxyId);

    // Extend AABB
    LvBoxBound b = aabb;
    LvVec3f r(aabbExtension);
    b.minp = b.minp - r;
    b.maxp = b.maxp + r;

    // Predict AABB displacement
    LvVec3f d = displacement * float(aabbExtension);

    if (d.x < 0.f)
        b.minp.x += d.x;
    else
        b.maxp.x += d.x;

    if (d.y < 0.f)
        b.minp.y += d.y;
    else
        b.maxp.y += d.y;

    if (d.z < 0.f)
        b.minp.z += d.z;
    else
        b.maxp.z += d.z;

    _nodes[proxyId].aabb = b;

    insertLeaf(proxyId);
    return true;
}

bool LvDynamicAABBTree::UpdateProxy(int proxyId, const LvBoxBound &aabb)
{
    LV_CHECK(0 <= proxyId && proxyId < _nodeCapacity, "Invalid Proxy Id");
    LV_CHECK(_nodes[proxyId].isLeaf(), "Invalid Proxy Id");

    if (_nodes[proxyId].aabb.Contains(aabb))
        return false;

    removeLeaf(proxyId);

    // Extend AABB
    LvBoxBound b = aabb;
    LvVec3f r(aabbExtension);
    b.minp = b.minp - r;
    b.maxp = b.maxp + r;
    _nodes[proxyId].aabb = b;

    insertLeaf(proxyId);
    return true;
}

void *LvDynamicAABBTree::GetUserData(int proxyId) const
{
    LV_CHECK(0 <= proxyId && proxyId < _nodeCapacity, "Invalid Proxy Id");
    return _nodes[proxyId].userData;
}

const LvBoxBound &LvDynamicAABBTree::GetFatAABB(int proxyId) const
{
    LV_CHECK(0 <= proxyId && proxyId < _nodeCapacity, "Invalid Proxy Id");
    return _nodes[proxyId].aabb;
}

int LvDynamicAABBTree::GetHeight() const
{
    if (_root == nodeNull)
        return 0;

    return _nodes[_root].height;
}

void LvDynamicAABBTree::Clear()
{
    _root = nodeNull;
    _nodeCount = 0;

    // Build a linked list for the free list
    for (int i = 0; i < _nodeCapacity - 1; ++i)
    {
        _nodes[i].next = i + 1;
        _nodes[i].height = -1;
    }
    _nodes[_nodeCapacity - 1].next = nodeNull;
    _nodes[_nodeCapacity - 1].height = -1;
    _freeList = 0;

    _insertionCount = 0;
}

int LvDynamicAABBTree::allocateNode()
{
    // Expand the node pool as needed.
    if (_freeList == nodeNull)
    {
        LV_CHECK(_nodeCount == _nodeCapacity,
                 "node count should be full with node capcity");

        // The free list is empty. Rebuild a bigger pool
        TreeNode *oldNodes = _nodes;
        _nodeCapacity *= 2;
        _nodes = new TreeNode[_nodeCapacity];
        memcpy((void *)_nodes, (void *)oldNodes, _nodeCount * sizeof(TreeNode));
        delete[] oldNodes;
        oldNodes = nullptr;

        // Build a linked list for the free list. The parent
        // pointer becomes the "next" pointer.
        for (int i = _nodeCount; i < _nodeCapacity - 1; ++i)
        {
            _nodes[i].next = i + 1;
            _nodes[i].height = -1;
        }
        _nodes[_nodeCapacity - 1].next = nodeNull;
        _nodes[_nodeCapacity - 1].height = -1;
        _freeList = _nodeCount;
    }

    // Peel a node off the free list
    int nodeId = _freeList;
    _freeList = _nodes[nodeId].next;
    _nodes[nodeId].parent = nodeNull;
    _nodes[nodeId].left = nodeNull;
    _nodes[nodeId].right = nodeNull;
    _nodes[nodeId].height = 0;
    _nodes[nodeId].userData = nullptr;
    ++_nodeCount;
    return nodeId;
}

void LvDynamicAABBTree::freeNode(int nodeId)
{
    LV_CHECK(0 <= nodeId && nodeId < _nodeCapacity, "Invalid node id");
    LV_CHECK(0 < _nodeCount, "Invalid Node Count");
    _nodes[nodeId].next = _freeList;
    _nodes[nodeId].height = -1;
    _freeList = nodeId;
    --_nodeCount;
}

void LvDynamicAABBTree::insertLeaf(int leaf)
{
    ++_insertionCount;

    if (_root == nodeNull)
    {
        _root = leaf;
        _nodes[_root].parent = nodeNull;
        return;
    }

    // Find the best sibling for this node with Surface Area Heuristic
    LvBoxBound leafAABB = _nodes[leaf].aabb;
    int index = _root;
    while (_nodes[index].isLeaf() == false)
    {
        int left = _nodes[index].left;
        int right = _nodes[index].right;

        // Cost Heuristic Traversal on tree
        float area = _nodes[index].aabb.GetPerimeter();
        LvBoxBound combinedAABB;
        combinedAABB.ExpandToInclude(_nodes[index].aabb, leafAABB);
        float combinedArea = combinedAABB.GetPerimeter();

        // Cost of creating a new parent for this node and the new leaf
        float cost = 2.f * combinedArea;

        // Minimum cost of pushing the leaf further down the three
        float inheritanceCost = 2.f * (combinedArea - area);

        // Cost of descending into left child
        float cost1;
        if (_nodes[left].isLeaf())
        {
            LvBoxBound aabb;
            aabb.ExpandToInclude(_nodes[left].aabb, leafAABB);
            cost1 = aabb.GetPerimeter() + inheritanceCost;
        }
        else
        {
            LvBoxBound aabb;
            aabb.ExpandToInclude(_nodes[left].aabb, leafAABB);
            float oldArea = _nodes[left].aabb.GetPerimeter();
            float newArea = aabb.GetPerimeter();
            cost1 = (newArea - oldArea) + inheritanceCost;
        }

        // Cost of descending into right child
        float cost2;
        if (_nodes[right].isLeaf())
        {
            LvBoxBound aabb;
            aabb.ExpandToInclude(_nodes[right].aabb, leafAABB);
            cost2 = aabb.GetPerimeter() + inheritanceCost;
        }
        else
        {
            LvBoxBound aabb;
            aabb.ExpandToInclude(_nodes[right].aabb, leafAABB);
            float oldArea = _nodes[right].aabb.GetPerimeter();
            float newArea = aabb.GetPerimeter();
            cost2 = (newArea - oldArea) + inheritanceCost;
        }

        // Descend according to the minimum cost
        if (cost < cost1 && cost < cost2)
            break;

        // Descend
        if (cost1 < cost2)
            index = left;
        else
            index = right;
    }

    int sibling = index;

    // Create a new parent
    int oldParent = _nodes[sibling].parent;
    int newParent = allocateNode();
    _nodes[newParent].parent = oldParent;
    _nodes[newParent].userData = nullptr;
    _nodes[newParent].aabb.ExpandToInclude(_nodes[sibling].aabb, leafAABB);
    _nodes[newParent].height = _nodes[sibling].height + 1;

    if (oldParent != nodeNull)
    {
        // The sibling was not the root
        if (_nodes[oldParent].left == sibling)
        {
            _nodes[oldParent].left = newParent;
        }
        else
        {
            _nodes[oldParent].right = newParent;
        }

        _nodes[newParent].left = sibling;
        _nodes[newParent].right = leaf;
        _nodes[sibling].parent = newParent;
        _nodes[leaf].parent = newParent;
    }
    else
    {
        // The sibling was the root.
        _nodes[newParent].left = sibling;
        _nodes[newParent].right = leaf;
        _nodes[sibling].parent = newParent;
        _nodes[leaf].parent = newParent;
        _root = newParent;
    }

    // Walk back up the tree fixing heights and AABBs
    index = _nodes[leaf].parent;
    while (index != nodeNull)
    {
        index = makeBalance(index);

        int left = _nodes[index].left;
        int right = _nodes[index].right;

        ASSERT(left != nodeNull);
        ASSERT(right != nodeNull);

        _nodes[index].height =
            1 + LV_MAX(_nodes[left].height, _nodes[right].height);
        _nodes[index].aabb.ExpandToInclude(_nodes[left].aabb,
                                           _nodes[right].aabb);

        index = _nodes[index].parent;
    }
}

void LvDynamicAABBTree::removeLeaf(int leaf)
{
    if (leaf == _root)
    {
        _root = nodeNull;
        return;
    }
    int parent = _nodes[leaf].parent;
    int grandParent = _nodes[parent].parent;

    int sibling;
    if (_nodes[parent].left == leaf)
        sibling = _nodes[parent].right;
    else
        sibling = _nodes[parent].left;

    // parent is not the root.
    if (grandParent != nodeNull)
    {
        // Destroy parent and connect sibling to grandParnet
        if (_nodes[grandParent].left == parent)
            _nodes[grandParent].left = sibling;
        else
            _nodes[grandParent].right = sibling;

        _nodes[sibling].parent = grandParent;
        freeNode(parent);

        // Adjust ancestor bounds
        int index = grandParent;
        while (index != nodeNull)
        {
            index = makeBalance(index);

            int left = _nodes[index].left;
            int right = _nodes[index].right;

            _nodes[index].aabb.ExpandToInclude(_nodes[left].aabb,
                                               _nodes[right].aabb);
            _nodes[index].height =
                1 + LV_MAX(_nodes[left].height, _nodes[right].height);

            index = _nodes[index].parent;
        }
    }
    // parent is the root.
    else
    {
        _root = sibling;
        _nodes[sibling].parent = nodeNull;
        freeNode(parent);
    }
}

int LvDynamicAABBTree::makeBalance(int iA)
{
    LV_CHECK(iA != nodeNull, "Invalid Node Index");

    TreeNode *nodeA = _nodes + iA;
    if (nodeA->isLeaf() || nodeA->height < 2)
    {
        return iA;
    }

    int iB = nodeA->left;
    int iC = nodeA->right;
    LV_CHECK(0 <= iB && iB < _nodeCapacity, "Invalid Node Index");
    LV_CHECK(0 <= iC && iC < _nodeCapacity, "Invalid Node Index");

    TreeNode *nodeB = _nodes + iB;
    TreeNode *nodeC = _nodes + iC;

    int balance = nodeC->height - nodeB->height;

    // Rotate C up
    if (balance > 1)
    {
        int iF = nodeC->left;
        int iG = nodeC->right;
        TreeNode *nodeF = _nodes + iF;
        TreeNode *nodeG = _nodes + iG;
        LV_CHECK(0 <= iF && iF < _nodeCapacity, "Invalid Node Index");
        LV_CHECK(0 <= iG && iG < _nodeCapacity, "Invalid Node Index");

        // Swap A and C
        nodeC->left = iA;
        nodeC->parent = nodeA->parent;
        nodeA->parent = iC;

        // A's old parent should point to C
        if (nodeC->parent != nodeNull)
        {
            if (_nodes[nodeC->parent].left == iA)
            {
                _nodes[nodeC->parent].left = iC;
            }
            else
            {
                LV_CHECK(_nodes[nodeC->parent].right == iA,
                         "Invalid Node Index");
                _nodes[nodeC->parent].right = iC;
            }
        }
        else
        {
            _root = iC;
        }

        // Rotate
        if (nodeF->height > nodeG->height)
        {
            nodeC->right = iF;
            nodeA->right = iG;
            nodeG->parent = iA;
            nodeA->aabb.ExpandToInclude(nodeB->aabb, nodeG->aabb);
            nodeC->aabb.ExpandToInclude(nodeA->aabb, nodeF->aabb);

            nodeA->height = 1 + LV_MAX(nodeB->height, nodeG->height);
            nodeC->height = 1 + LV_MAX(nodeA->height, nodeF->height);
        }
        else
        {
            nodeC->right = iG;
            nodeA->right = iF;
            nodeF->parent = iA;
            nodeA->aabb.ExpandToInclude(nodeB->aabb, nodeF->aabb);
            nodeC->aabb.ExpandToInclude(nodeA->aabb, nodeG->aabb);

            nodeA->height = 1 + LV_MAX(nodeB->height, nodeF->height);
            nodeC->height = 1 + LV_MAX(nodeA->height, nodeG->height);
        }

        return iC;
    }

    // Rotate B up
    if (balance < -1)
    {
        int iD = nodeB->left;
        int iE = nodeB->right;
        TreeNode *nodeD = _nodes + iD;
        TreeNode *nodeE = _nodes + iE;
        LV_CHECK(0 <= iD && iD < _nodeCapacity, "Invalid Node Index");
        LV_CHECK(0 <= iE && iE < _nodeCapacity, "Invalid Node Index");

        // Swap A and B
        nodeB->left = iA;
        nodeB->parent = nodeA->parent;
        nodeA->parent = iB;

        // A's old parent should point to B
        if (nodeB->parent != nodeNull)
        {
            if (_nodes[nodeB->parent].left == iA)
            {
                _nodes[nodeB->parent].left = iB;
            }
            else
            {
                LV_CHECK(_nodes[nodeB->parent].right == iA,
                         "Invalid Node Index");
                _nodes[nodeB->parent].right = iB;
            }
        }
        else
        {
            _root = iB;
        }

        // Rotate
        if (nodeD->height > nodeE->height)
        {
            nodeB->right = iD;
            nodeA->left = iE;
            nodeE->parent = iA;
            nodeA->aabb.ExpandToInclude(nodeC->aabb, nodeE->aabb);
            nodeB->aabb.ExpandToInclude(nodeA->aabb, nodeD->aabb);

            nodeA->height = 1 + LV_MAX(nodeC->height, nodeE->height);
            nodeB->height = 1 + LV_MAX(nodeA->height, nodeD->height);
        }
        else
        {
            nodeB->right = iE;
            nodeA->left = iD;
            nodeD->parent = iA;
            nodeA->aabb.ExpandToInclude(nodeC->aabb, nodeD->aabb);
            nodeB->aabb.ExpandToInclude(nodeA->aabb, nodeE->aabb);

            nodeA->height = 1 + LV_MAX(nodeC->height, nodeD->height);
            nodeB->height = 1 + LV_MAX(nodeA->height, nodeE->height);
        }

        return iB;
    }

    return iA;
}
LV_NS_ENGINE_END
