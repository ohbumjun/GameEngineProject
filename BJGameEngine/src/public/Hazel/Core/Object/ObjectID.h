#pragma once

/*
// This needs to add up to 63, 1 bit is for reference.
#define OBJECTDB_VALIDATOR_BITS 39
#define OBJECTDB_VALIDATOR_MASK ((uint64_t(1) << OBJECTDB_VALIDATOR_BITS) - 1)
#define OBJECTDB_SLOT_MAX_COUNT_BITS 24
#define OBJECTDB_SLOT_MAX_COUNT_MASK ((uint64_t(1) << OBJECTDB_SLOT_MAX_COUNT_BITS) - 1)
#define OBJECTDB_REFERENCE_BIT (uint64_t(1) << (OBJECTDB_SLOT_MAX_COUNT_BITS + OBJECTDB_VALIDATOR_BITS))
*/

namespace Hazel
{
	class ObjectID {

		// [1 == reference ][39 == validator ][24 == slot Á¤º¸]
		uint64_t m_ID = 0;

	public:
		inline ObjectID() = default;
		inline bool IsRefCounted() const { return (m_ID & (uint64_t(1) << 63)) != 0; }
		inline bool IsValid() const { return m_ID != 0; }
		inline bool IsNull() const { return m_ID == 0; }
		inline operator uint64_t() const { return m_ID; }
		inline operator int64_t() const { return m_ID; }
		inline bool operator==(const ObjectID& p_id) const { return m_ID == p_id.m_ID; }
		inline bool operator!=(const ObjectID& p_id) const { return m_ID != p_id.m_ID; }
		inline bool operator<(const ObjectID& p_id) const { return m_ID < p_id.m_ID; }
		inline void operator=(int64_t p_int64) { m_ID = p_int64; }
		inline void operator=(uint64_t p_uint64) { m_ID = p_uint64; }
		inline explicit ObjectID(const uint64_t p_id) { m_ID = p_id; }
		inline explicit ObjectID(const int64_t p_id) { m_ID = p_id; }
	};

}