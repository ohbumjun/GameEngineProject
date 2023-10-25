#pragma once
#include "MemoryPoolAllocator.h"
#include "PoolAllocator.h"
#include "StackAllocator.h"
#include "FreeListAllocator.h"
#include "MemoryPoolInfo.h"

class MemoryPool 
{
    friend class MemoryPoolManager; 
public:
    

    // void* Allocate(const size_t allocateSize);
    void* AllocateFromPoolAlloc();
    void* Allocate(const size_t allocateSize);
    template<typename T>
    void Free(T* ptr)
    {
        // �Ҹ��� ȣ��
        ptr->~T();

        m_Allocator->Free(ptr);
    }
public:
    MemoryPoolType GetAllocatorType()
    {
        return m_AllocatorType;
    }
    void SetName(std::string_view name)
    {
        m_Name = name;
    }
protected:
    MemoryPool();
    virtual ~MemoryPool();
    int m_Alignment;
    int m_AllocNumber; // ��� Object �� �Ҵ��� ���ΰ�
    size_t m_SingleDataSize;
    class MemoryPoolAllocator* m_Allocator;
    MemoryPoolType m_AllocatorType;
    std::string m_Name;

protected:
    bool InitAlloc(const int TotalMemorySize, MemoryPoolType Type, const int alignment = MemoryPoolInfo::ALIGNMENT);
    
    template<typename T>
    bool InitPoolAlloc(const int TotalNumber)
    {
        size_t AllocTotalSize = (size_t)(TotalNumber * sizeof(T));

        m_Allocator = new CPoolAllocator(AllocTotalSize, sizeof(T));

        m_AllocatorType = MemoryPoolType::Pool;

        m_Alignment = MemoryPoolInfo::ALIGNMENT;

        if (m_Allocator == nullptr)
            assert(false);

        m_SingleDataSize = sizeof(T);

        m_Allocator->Init();

        return true;
    }
};

