#pragma once

class BaseMemoryPool 
{
    friend class MemoryPoolManager; 
public:
    virtual void* Allocate(const size_t allocateSize) = 0;
    template<typename T>
    void Free(T* ptr) 
    {
        ptr->~T();

        onFree(ptr);
    }
public:
protected:
    BaseMemoryPool(){}
    virtual ~BaseMemoryPool(){}
    virtual void onFree(void* ptr) = 0;
    // int m_Alignment;
    // int m_AllocNumber; 
};

