
#include "Hazel/Core/GC/GCObject.h"
#include "hzpch.h"


GCObject::GCObject(void *data,
                   GCAllocationHeader *header,
                   TypeInfo *pTypeInfo)
{
    m_DataPtr = data;
    m_HeaderAddr = header;
    m_TypeInfo = pTypeInfo;
}

void GCObject::SetIsRoot(bool isRoot)
{
    m_HeaderAddr->isRoot = isRoot;
}

void GCObject::SetVisit(bool visit)
{
    m_HeaderAddr->visited = visit;
}

void GCObject::AddRef()
{
    m_HeaderAddr->refCnt += 1;
}

void GCObject::SetNext(GCObject *current, GCObject *next)
{
    assert(current->m_Next != next);

    current->m_Next = next;
}

bool GCObject::IsValid()
{
    if (m_DataPtr == nullptr)
        return false;

    // Dangling 검사

    return false;
}