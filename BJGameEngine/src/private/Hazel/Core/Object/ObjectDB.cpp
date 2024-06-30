#include "Hazel/Core/Object/ObjectDB.h"
#include "Hazel/Core/Object/BaseObject.h"
#include "Hazel/Core/Object/ObjectID.h"
#include "hzpch.h"
#include <random>

static std::random_device s_RandomDevice;
static std::mt19937_64 s_Engine(s_RandomDevice());
static std::uniform_int_distribution<uint32_t> s_UniformDistribution;

namespace Hazel
{

DefaultHeapAllocator ObjectDB::m_Allocator;
SpinLock ObjectDB::m_SpinLock;
uint32_t ObjectDB::m_SlotCnt;
uint32_t ObjectDB::m_SlotMax;
ObjectDB::ObjectSlot *ObjectDB::m_ObjectSlotArray;

void ObjectDB::Clean()
{
    if (m_ObjectSlotArray)
    {
        // memfree(m_ObjectSlotArray);
    }
}

ObjectID ObjectDB::Add(BaseObject *p_object)
{
    m_SpinLock.Lock();

    if (m_SlotCnt >= m_SlotMax)
    {
        HZ_CORE_ASSERT(m_SlotCnt < (1 << OBJECTDB_SLOT_MAX_COUNT_BITS),
                       "ObjectDB::Add() : m_SlotCnt >= m_SlotMax");
        // CRASH_COND(m_SlotCnt == (1 << OBJECTDB_m_SlotMax_COUNT_BITS));

        uint32_t newSlotMax = m_SlotMax > 0 ? m_SlotMax * 2 : 1;

        m_ObjectSlotArray = (ObjectSlot *)m_Allocator.Reallocate(
            m_ObjectSlotArray,
            sizeof(ObjectSlot) * newSlotMax);

        for (uint32_t i = m_SlotMax; i < newSlotMax; i++)
        {
            m_ObjectSlotArray[i].object = nullptr;
            m_ObjectSlotArray[i].nextFree = i;
            m_ObjectSlotArray[i].uniqueId = 0;
        }
        m_SlotMax = newSlotMax;
    }

    uint32_t slot = m_ObjectSlotArray[m_SlotCnt].nextFree;

    if (m_ObjectSlotArray[slot].object != nullptr)
    {
        m_SpinLock.Unlock();
        // ERR_FAIL_COND_V(m_ObjectSlotArray[slot].object != nullptr, ObjectID());
    }

    m_ObjectSlotArray[slot].object = p_object;

    m_ObjectSlotArray[slot].uniqueId = s_UniformDistribution(s_Engine);

    uint64_t id = m_ObjectSlotArray[slot].uniqueId;

    id <<= OBJECTDB_SLOT_MAX_COUNT_BITS;

    id |= uint64_t(slot);

    m_SlotCnt++;

    m_SpinLock.Unlock();

    return ObjectID(id);
}

void ObjectDB::Remove(BaseObject *p_object)
{
    uint64_t t = p_object->GetInstanceID();
    uint32_t slot =
        t & OBJECTDB_SLOT_MAX_COUNT_MASK; //slot is always valid on valid object

    m_SpinLock.Lock();

    // #ifdef DEBUG_ENABLED

    if (m_ObjectSlotArray[slot].object != p_object)
    {

        m_SpinLock.Unlock();

        // ERR_FAIL_COND(m_ObjectSlotArray[slot].object != p_object);

        HZ_CORE_ASSERT(
            m_ObjectSlotArray[slot].object == p_object,
            "ObjectDB::Remove() : m_ObjectSlotArray[slot].object != p_object");
    }

    uint64_t validator =
        (t >> OBJECTDB_SLOT_MAX_COUNT_BITS) & OBJECTDB_VALIDATOR_MASK;

    if (m_ObjectSlotArray[slot].uniqueId != validator)
    {
        m_SpinLock.Unlock();

        // ERR_FAIL_COND(m_ObjectSlotArray[slot].valid_num != validator);

        HZ_CORE_ASSERT(m_ObjectSlotArray[slot].uniqueId == validator,
                       "ObjectDB::Remove() : m_ObjectSlotArray[slot].valid_num "
                       "!= validator");
    }

    // #endif

    //decrease slot count
    m_SlotCnt--;

    //set the free slot properly
    m_ObjectSlotArray[m_SlotCnt].nextFree = slot;

    //invalidate, so checks against it fail
    m_ObjectSlotArray[slot].uniqueId = 0;
    m_ObjectSlotArray[slot].object = nullptr;

    m_SpinLock.Unlock();
}

int ObjectDB::GetObjectCount()
{
    return 0;
}

} // namespace Hazel