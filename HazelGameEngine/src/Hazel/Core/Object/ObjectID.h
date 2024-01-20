#pragma once

class ObjectID {
	uint64_t m_ID = 0;

public:
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



