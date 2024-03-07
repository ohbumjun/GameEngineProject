#pragma once
/**************************************************************************/
/*  object.h                                                              */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "Hazel/Core/Allocation/Allocator/DefaultHeapAllocator.h"
#include "Hazel/Core/Object/ObjectID.h"
#include "Hazel/Core/Thread/ThreadUtil.h"

namespace Hazel
{
class BaseObject;

class ObjectDB
{
    friend class BaseObject;

    // This needs to add up to 63, 1 bit is for reference.
#define OBJECTDB_VALIDATOR_BITS 39
#define OBJECTDB_VALIDATOR_MASK ((uint64_t(1) << OBJECTDB_VALIDATOR_BITS) - 1)
#define OBJECTDB_SLOT_MAX_COUNT_BITS 24
#define OBJECTDB_SLOT_MAX_COUNT_MASK                                           \
    ((uint64_t(1) << OBJECTDB_SLOT_MAX_COUNT_BITS) - 1)
#define OBJECTDB_REFERENCE_BIT                                                 \
    (uint64_t(1) << (OBJECTDB_SLOT_MAX_COUNT_BITS + OBJECTDB_VALIDATOR_BITS))


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
        if (m_ObjectSlotArray[slot].valid_num != validator)
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
        uint64_t valid_num : OBJECTDB_VALIDATOR_BITS;
        uint64_t next_free : OBJECTDB_SLOT_MAX_COUNT_BITS;
        uint64_t is_ref_counted : 1;
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
    static uint64_t m_ValidateNumber;
};
} // namespace Hazel
