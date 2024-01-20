#include "hzpch.h"
#include "ObjectDB.h"
#include "BaseObject.h"
#include "ObjectID.h"

void ObjectDB::Clean()
{
	if (m_ObjectSlotArray) {
		// memfree(m_ObjectSlotArray);
	}
}

ObjectID ObjectDB::Add(BaseObject* p_object)
{
	m_SpinLock.lock();
	if (m_SlotCnt >= m_SlotMax)
	{
		// CRASH_COND(m_SlotCnt == (1 << OBJECTDB_m_SlotMax_COUNT_BITS));

		uint32_t new_m_SlotMax = m_SlotMax > 0 ? m_SlotMax * 2 : 1;
		m_ObjectSlotArray = (ObjectSlot*)memrealloc(m_ObjectSlotArray, sizeof(ObjectSlot) * new_m_SlotMax);
		for (uint32_t i = m_SlotMax; i < new_m_SlotMax; i++) {
			m_ObjectSlotArray[i].object = nullptr;
			m_ObjectSlotArray[i].is_ref_counted = false;
			m_ObjectSlotArray[i].next_free = i;
			m_ObjectSlotArray[i].validator = 0;
		}
		m_SlotMax = new_m_SlotMax;
	}

	uint32_t slot = m_ObjectSlotArray[m_SlotCnt].next_free;
	if (m_ObjectSlotArray[slot].object != nullptr) {
		m_SpinLock.unlock();
		// ERR_FAIL_COND_V(m_ObjectSlotArray[slot].object != nullptr, ObjectID());
	}
	m_ObjectSlotArray[slot].object = p_object;
	m_ObjectSlotArray[slot].is_ref_counted = p_object->IsRefCounted();
	m_ValidateCounter = (m_ValidateCounter + 1) & OBJECTDB_VALIDATOR_MASK;

	// if (unlikely(m_ValidateCounter == 0)) {
	if (m_ValidateCounter == 0) 
	{
		m_ValidateCounter = 1;
	}
	m_ObjectSlotArray[slot].validator = m_ValidateCounter;

	uint64_t id = m_ValidateCounter;

	id <<= OBJECTDB_SLOT_MAX_COUNT_BITS;

	id |= uint64_t(slot);

	if (p_object->IsRefCounted()) {
		id |= OBJECTDB_REFERENCE_BIT;
	}

	m_SlotCnt++;

	m_SpinLock.unlock();

	return ObjectID(id);
}

void ObjectDB::Remove(BaseObject* p_object)
{
	uint64_t t = p_object->GetInstanceID();
	uint32_t slot = t & OBJECTDB_SLOT_MAX_COUNT_MASK; //slot is always valid on valid object

	m_SpinLock.lock();

#ifdef DEBUG_ENABLED

	if (m_ObjectSlotArray[slot].object != p_object) {
		m_SpinLock.unlock();
		ERR_FAIL_COND(m_ObjectSlotArray[slot].object != p_object);
	}
	{
		uint64_t validator = (t >> OBJECTDB_m_SlotMax_COUNT_BITS) & OBJECTDB_VALIDATOR_MASK;
		if (m_ObjectSlotArray[slot].validator != validator) {
			m_SpinLock.unlock();
			ERR_FAIL_COND(m_ObjectSlotArray[slot].validator != validator);
		}
	}

#endif
	//decrease slot count
	m_SlotCnt--;
	//set the free slot properly
	m_ObjectSlotArray[m_SlotCnt].next_free = slot;
	//invalidate, so checks against it fail
	m_ObjectSlotArray[slot].validator = 0;
	m_ObjectSlotArray[slot].is_ref_counted = false;
	m_ObjectSlotArray[slot].object = nullptr;

	m_SpinLock.unlock();
}

int ObjectDB::GetObjectCount()
{
    return 0;
}
