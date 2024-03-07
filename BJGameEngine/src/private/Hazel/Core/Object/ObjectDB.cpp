#include "Hazel/Core/Object/ObjectDB.h"
#include "Hazel/Core/Object/BaseObject.h"
#include "Hazel/Core/Object/ObjectID.h"
#include "hzpch.h"

namespace Hazel
{
DefaultHeapAllocator ObjectDB::m_Allocator;
SpinLock ObjectDB::m_SpinLock;
uint32_t ObjectDB::m_SlotCnt;
uint32_t ObjectDB::m_SlotMax;
ObjectDB::ObjectSlot *ObjectDB::m_ObjectSlotArray;
uint64_t ObjectDB::m_ValidateNumber;

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

        uint32_t new_m_SlotMax = m_SlotMax > 0 ? m_SlotMax * 2 : 1;

        m_ObjectSlotArray = (ObjectSlot *)m_Allocator.Reallocate(
            m_ObjectSlotArray,
            sizeof(ObjectSlot) * new_m_SlotMax);

        for (uint32_t i = m_SlotMax; i < new_m_SlotMax; i++)
        {
            m_ObjectSlotArray[i].object = nullptr;
            m_ObjectSlotArray[i].is_ref_counted = false;
            m_ObjectSlotArray[i].next_free = i;
            m_ObjectSlotArray[i].valid_num = 0;
        }
        m_SlotMax = new_m_SlotMax;
    }

    uint32_t slot = m_ObjectSlotArray[m_SlotCnt].next_free;

    if (m_ObjectSlotArray[slot].object != nullptr)
    {
        m_SpinLock.Unlock();
        // ERR_FAIL_COND_V(m_ObjectSlotArray[slot].object != nullptr, ObjectID());
    }

    m_ObjectSlotArray[slot].object = p_object;
    m_ObjectSlotArray[slot].is_ref_counted = p_object->IsRefCounted();
    m_ValidateNumber = (m_ValidateNumber + 1) & OBJECTDB_VALIDATOR_MASK;

    // if (unlikely(m_ValidateCounter == 0)) {
    if (m_ValidateNumber == 0)
    {
        m_ValidateNumber = 1;
    }

    m_ObjectSlotArray[slot].valid_num = m_ValidateNumber;

    uint64_t id = m_ValidateNumber;

    id <<= OBJECTDB_SLOT_MAX_COUNT_BITS;

    id |= uint64_t(slot);

    if (p_object->IsRefCounted())
    {
        id |= OBJECTDB_REFERENCE_BIT;
    }

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

    if (m_ObjectSlotArray[slot].valid_num != validator)
    {
        m_SpinLock.Unlock();

        // ERR_FAIL_COND(m_ObjectSlotArray[slot].valid_num != validator);

        HZ_CORE_ASSERT(m_ObjectSlotArray[slot].valid_num == validator,
                       "ObjectDB::Remove() : m_ObjectSlotArray[slot].valid_num "
                       "!= validator");
    }

    // #endif

    //decrease slot count
    m_SlotCnt--;

    //set the free slot properly
    m_ObjectSlotArray[m_SlotCnt].next_free = slot;

    //invalidate, so checks against it fail
    m_ObjectSlotArray[slot].valid_num = 0;
    m_ObjectSlotArray[slot].is_ref_counted = false;
    m_ObjectSlotArray[slot].object = nullptr;

    m_SpinLock.Unlock();
}

int ObjectDB::GetObjectCount()
{
    return 0;
}

} // namespace Hazel