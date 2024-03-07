#pragma once

#include "../Reflection/Reflection.h"
#include "GCInfo.h"

struct TypeInfo;

class GCObject
{
public:
    GCObject(void *data, GCAllocationHeader *header, TypeInfo *pTypeInfo);

    void SetIsRoot(bool isRoot);
    void SetVisit(bool visit);
    void AddRef();
    void SetNext(GCObject *current, GCObject *next);
    // nullptr 인지 혹은 Dangling 인지를 검사하는 함수
    bool IsValid();

    inline TypeInfo *GetTypeInfo() const
    {
        return m_TypeInfo;
    }
    inline GCObject *GetNext() const
    {
        return m_Next;
    }
    inline void *GetDataPtr() const
    {
        return m_DataPtr;
    }
    inline bool IsVisited()
    {
        return false;
    }
    inline bool IsRoot()
    {
        return false;
    }
    inline int GetRefCount()
    {
        return 0;
    }
    inline TypeId GetType()
    {
        return m_TypeInfo->m_Type;
    };

private:
    // _object_db_rec_ 들을 연결리스트 형태로 관리할 것이다.
    // 그 다음 노드에 대한 포인터
    GCObject *m_Next;

    // 할당한 object 의 메모리 주소
    // _object_db_ 에서 해당 object_rec 를 찾는 key 로 사용할 것이다.
    void *m_DataPtr;

    // 또한 HeaderAddress 도 가지고 있을 수 있게 해야 한다.
    // void* headerAddr;
    GCAllocationHeader *m_HeaderAddr;

    // 해당 object 가 속한 structure_db_rec 객체를 가리키는 포인터
    // xalloc 시 들어온 "string" 값을 이용하여 struct_db 에서 찾아내서 링크시켜주기
    // struct_db_rec_t* struct_rec;
    TypeInfo *m_TypeInfo;

    // 아래 정보들은 Header 쪽에 배치해보자.
    // bool is_visited; /*Used for Graph traversal*/
    // bool is_root;
    // int   refCount;  /*Used for Graph traversal*/
};
