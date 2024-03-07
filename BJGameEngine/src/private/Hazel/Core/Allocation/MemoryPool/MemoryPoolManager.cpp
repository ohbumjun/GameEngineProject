#include "Hazel/Core/Allocation/MemoryPool/MemoryPoolManager.h"
#include "Hazel/Core/Allocation/MemoryPool/FixedMemoryPool.h"
#include "Hazel/Core/Allocation/MemoryPool/VariantMemoryPool.h"
#include "hzpch.h"

MemoryPoolManager::MemoryPoolManager()
{
    m_FixedPool = new FixedMemoryPool;
    m_FixedPool->Init(4096, m_FixedSize);

    // FreeList
    m_VariantPool = new VariantMemoryPool;
    m_VariantPool->Init(4096 * 2);
}

MemoryPoolManager::~MemoryPoolManager()
{
    if (m_FixedPool)
        delete m_FixedPool;
    if (m_VariantPool)
        delete m_VariantPool;
}
