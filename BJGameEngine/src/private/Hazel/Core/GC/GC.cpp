
#include "Hazel/Core/GC/GC.h"
#include "Hazel/Core/GC/GCObject.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "hzpch.h"

GC::GC()
{
    // 인자로 TotalSize 는 사실 의미에 맞지 않는다.
    // GCAllocator 안에 PoolAllocator , freeList Allocator
    // 2개가 있는데 이 중에서 FreeList 만 사용하는 size 이다.
    m_GCAllocator = new MemoryPoolManager();
    m_GCAllocator->SetFixedSize(sizeof(GCObject));
}

GC::~GC()
{
    delete m_GCAllocator;
    m_GCAllocator = nullptr;
}

void GC::Run()
{
    mark();
    sweep();
}
void GC::AddCollectTarget(GCObject *object)
{
    m_CollectTargets.push_back(object);
}

GCObject *GC::FindGCObject(void *dataPtr)
{
    auto iter = m_CollectTargets.begin();
    auto iterEnd = m_CollectTargets.end();

    for (; iter != iterEnd; ++iter)
    {
        GCObject *curGCObject = *iter;

        if (curGCObject->GetDataPtr() == dataPtr)
            return curGCObject;
    }

    return nullptr;
}

template <typename T>
void GC::Allocate(bool isRoot)
{
    // isRoot 은 GCObject 를 리턴받고 세팅해줄 것이다.
    m_GCAllocator->Allocate<T>();
}

void GC::Free(GCObject *gcObject)
{
    m_GCAllocator->Free<GCObject>(static_cast<void *>(gcObject));
}

void GC::reset()
{
    // GCObject* obj_rec = object_db->head;

    auto iter = m_CollectTargets.begin();
    auto iterEnd = m_CollectTargets.end();

    for (iter; iter != iterEnd; ++iter)
    {
        GCObject *object = *iter;
        object->SetVisit(false);
    }
}

void GC::mark()
{
    /*Step 1 : Mark all objects in object databse as unvisited*/
    reset();

    /* Step 2 : Get the "first root object" from the object db, it could be
     * present anywhere in object db. If there are multiple roots in object db
     * return the first one, we can start mld algorithm from any root object*/

    GCObject *root_obj = getFirstRootObject();

    while (root_obj)
    {

        // 이미 방문한 root node 재귀 방지
        if (root_obj->IsVisited())
        {
            //  해당 root object 로부터 시작되는 graph 내의 모든 노드는 탐색이 끝난 상황
            root_obj = getNextRootObject(root_obj);
            continue;
        }
        // dangling pointer handling
        else if (root_obj->GetDataPtr() == nullptr)
        {
            root_obj = getNextRootObject(root_obj);
            continue;
        }

        root_obj->SetVisit(true);

        /*Explore all reachable objects from this root_obj recursively*/
        markRecursively(root_obj);

        root_obj = getNextRootObject(root_obj);
    }
}

void GC::sweep()
{
    auto iter = m_CollectTargets.begin();
    auto iterEnd = m_CollectTargets.end();

    GCObject *prevObject = nullptr;

    for (; iter != iterEnd;)
    {
        GCObject *gcObject = *iter;

        if (gcObject->IsVisited() == false)
        {
            // next 관계 변경해주기
            if (prevObject)
            {
                prevObject->SetNext(prevObject, gcObject->GetNext());
            }

            // 메모리 해제해주기
            m_GCAllocator->Free<GCObject>(prevObject);

            // list 상에서 지워주기
            iter = m_CollectTargets.erase(iter);
        }
        else
        {
            ++iter;

            // prevObject 는 메모리 해제되지 않은 대상을 가리켜야 하므로
            // else 문쪽으로 옮긴다.
            prevObject = gcObject;
        }
    }
}

GCObject *GC::getFirstRootObject()
{
    auto iter = m_CollectTargets.begin();
    auto iterEnd = m_CollectTargets.end();

    for (iter; iter != iterEnd; ++iter)
    {
        GCObject *object = *iter;

        if (object->IsRoot())
        {
            return object;
        }
    }

    return nullptr;
}

GCObject *GC::getNextRootObject(GCObject *curGCObject)
{
    if (curGCObject->GetNext() == nullptr)
        return nullptr;

    return curGCObject->GetNext();
}

void GC::markRecursively(GCObject *parentObject)
{
    void *parent_obj_ptr = nullptr;
    // * child_obj_offset = NULL;

    void *child_object_ptr = nullptr;
    FieldInfo *field_info = NULL;

    GCObject *child_object = NULL;
    Reflection::TypeInfo *parentTypeInfo = parentObject->GetTypeInfo();

    /*Parent object must have already visited*/
    assert(parentObject->IsVisited());

    if (parentTypeInfo->m_fieldInfos.size() == 0)
    {
        return;
    }

    // 현재 조사중인 object 의 메모리 주소에 접근한다.
    parent_obj_ptr = parentObject->GetDataPtr();

    // 해당 object type 의 모든 field 를 순회한다.
    auto iter = parentTypeInfo->m_fieldInfos.begin();
    auto iterEnd = parentTypeInfo->m_fieldInfos.end();

    for (; iter != iterEnd; ++iter)
    {
        const FieldInfo *fieldInfo = &(*iter);

        DataType fieldDataType =
            Reflection::GetDataType(fieldInfo->GetTypeId());

        if (fieldDataType != DataType::OBJECT)
            continue;

        /* 현재 parent object 가 직접 가리키고 있는 pointer object 가 바로 child object 이다.
              ex) parent_obj_ptr = 0x1000 = parent object 의 메모리 주소 를 담기 위해 쓰인다.
                  그리고 parent object 가 이와 같은 형태를 취했다고 해보자. {[float][int][char[30]][0x2000]}

                  이때 [0x2000] 은, chlid object pointer 변수이다.
                  그리고 [0x2000] 이라는 데이터의 메모리 주소가 "0x1010" 이라고 해보자.

                  child_obj_offset = 0x1010 이 될 것이다.

              * child_obj_offset is the memory location inside parent object
              * where address of next level object is stored*/
        uint64_t child_obj_offset_value =
            (uint64_t)parent_obj_ptr + field_info->GetOffset();
        char *child_obj_offset =
            reinterpret_cast<char *>(child_obj_offset_value);

        /*
            child_object_address 변수에 담는 값은 무엇이 될까 ?

            child_obj_offset 는 자세히 살펴보면 "이중 포인터" 이다.
            *(child_obj_offset) == [0x2000] 이라는 데이터 == child object 를 가리키는 포인터
            **(child_obj_offset) == *([0x2000] 이라는 데이터) == *(child object 를 가리키는 포인터) == child 객체

            &child_object_address 라는 것은, child_object_address = ??, 를 수행한다는 것.
            즉, 특정 값을 child_object_address 변수에 대입한다는 의미

            child_obj_offset ?
            보통은 &child_obj_offset 형태로 쓰는데, 여기서는 & 를 빼고
            child_obj_offset 를 사용했다.
            child_obj_offset 는 이중포인터 라고 했다
            child_obj_offset = &[0x2000]

            memcpy(&child_object_address, &[0x2000], sizeof(void*)); 라는 의미는
            child_object_address = 0x2000 을 한다는 것이고

            이 말은 즉슨, child_object_address 에 chlid 객체의 메모리 주소. 값을 대입한다는 것이고
            *(child_object_address) = child object. 가 된다는 것이다.
        */
        memcpy(&child_object_ptr, child_obj_offset, sizeof(void *));

        /*child_object_address now stores the address of the next object in the
            * graph. It could be NULL, Handle that as well*/
        if (!child_object_ptr)
        {
            continue;
        }

        GCObject *childObject = GC::FindGCObject(child_object_ptr);

        assert(childObject);

        /* Since we are able to reach this child object "child_object_rec"
            * from parent object "parent_obj_ptr", mark this
            * child object as visited and explore its children recirsively.
            * If this child object is already visited, then do nothing - avoid infinite loops*/
        if (childObject->IsVisited())
        {
            continue;
        }

        childObject->SetVisit(true);

        markRecursively(childObject);
    }
}