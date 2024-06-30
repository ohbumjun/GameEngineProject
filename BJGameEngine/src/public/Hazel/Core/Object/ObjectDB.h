#pragma once

#include "Hazel/Core/Allocation/Allocator/DefaultHeapAllocator.h"
#include "Hazel/Core/Object/ObjectID.h"
#include "Hazel/Core/Thread/ThreadUtil.h"

namespace Hazel
{
class BaseObject;

class ObjectDB
{
    friend class BaseObject;

// @brief [32bit] validator, [32bit] slot index
#define OBJECTDB_VALIDATOR_BITS 32
#define OBJECTDB_VALIDATOR_MASK ((uint64_t(1) << OBJECTDB_VALIDATOR_BITS) - 1)
#define OBJECTDB_SLOT_MAX_COUNT_BITS 32
#define OBJECTDB_SLOT_MAX_COUNT_MASK                                           \
    ((uint64_t(1) << OBJECTDB_SLOT_MAX_COUNT_BITS) - 1)


public:
    inline static BaseObject *GetObjectInstance(ObjectID p_instance_id)
    {
        uint64_t id = p_instance_id;
        uint32_t slot = id & OBJECTDB_SLOT_MAX_COUNT_MASK;

        // 굳이 spin lock 을 걸어주는 이유가 있나 ?
        // m_SpinLock.lock();

        uint64_t validator =
            (id >> OBJECTDB_SLOT_MAX_COUNT_BITS) & OBJECTDB_VALIDATOR_MASK;

        // if (unlikely(object_slots[slot].validator != validator)) {
        if (m_ObjectSlotArray[slot].uniqueId != validator)
        {
            // m_SpinLock.unlock();
            return nullptr;
        }

        BaseObject *object = m_ObjectSlotArray[slot].object;

        // m_SpinLock.unlock();

        return object;
    }
    static int get_object_count();

private:
    struct ObjectSlot
    { // 128 bits per slot.
        // 해당 slot 에 들어있는 address 가, 기존 object id 와 동일한 녀석인지.
        uint64_t uniqueId : OBJECTDB_VALIDATOR_BITS;
        uint64_t nextFree : OBJECTDB_SLOT_MAX_COUNT_BITS;
        BaseObject *object = nullptr;
    };

    int GetObjectCount();
    static void Clean();

    static ObjectID Add(BaseObject *p_object);
    static void Remove(BaseObject *p_object);

    static DefaultHeapAllocator m_Allocator;
    static SpinLock m_SpinLock;
    static uint32_t m_SlotCnt;
    static uint32_t m_SlotMax;
    static ObjectSlot *m_ObjectSlotArray;
};
} // namespace Hazel
