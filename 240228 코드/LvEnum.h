#pragma once
#ifndef __LV_ENUM_H__
#define __LV_ENUM_H__

#include "system/LvReflection.h"

LV_NS_BEGIN

class LV_API LvEnum
{
public:
	/**
	 * @brief Enum 이름을 반환합니다.
	 */
	template<typename T>
	static const char* GetName(T enumValue)
	{
		static_assert(std::is_enum<T>::value, "T should be only a enum");
		static LvTypeId type = LV_TYPEOF(T);
		return GetName(type, static_cast<uint64>(enumValue));
	}

	/**
	 * @brief enum 이름으로 값을 반환합니다.
	 */
	template<typename T>
	static T GetValue(const char* enumName)
	{
		static_assert(std::is_enum<T>::value, "T should be only a enum");
		static LvTypeId type = LV_TYPEOF(T);
		return static_cast<T>(GetValue(type, enumName));
	}

	/**
	 * @brief Enum 타입안에 해당 멤버가 있는지 확인합니다.
	 */
	template<typename T>
	static bool Contain(const char* enumName)
	{
		static_assert(std::is_enum<T>::value, "T should be only a enum");
		static LvTypeId type = LV_TYPEOF(T);
		return Contain(type, enumName);

	}

	/**
	 * @brief Enum 이름으로 값을 가져오는 시도를 합니다.
	 */
	template<typename T>
	static bool TryGetValue(const char* enumName, T& outValue)
	{
		static_assert(std::is_enum<T>::value, "T should be only a enum");
		static LvTypeId type = LV_TYPEOF(T);

		uint64 value;
		if (TryGetValue(type, enumName, value))
		{
			outValue = static_cast<T>(value);
			return true;
		}
		return false;
	}

	/**
	 * @brief Enum 이름이나 값으로 가져오는 시도를 합니다.
	 */
	template<typename T>
	static bool TryParse(const char* valueOrName, T& outValue)
	{
		static_assert(std::is_enum<T>::value, "T should be only a enum");
		static LvTypeId type = LV_TYPEOF(T);

		uint64 value;
		if (TryParse(type, valueOrName, value))
		{
			outValue = static_cast<T>(value);
			return true;
		}
		return false;
	}

	static bool Contain(LvTypeId enumType, const char* enumName);

	static const char* GetName(LvTypeId enumType, uint64 enumValue);

	static uint64 GetValue(LvTypeId enumType, const char* enumName);

	static bool TryGetValue(LvTypeId enumType, const char* enumName, uint64& outValue);

	static bool TryParse(LvTypeId enumType, const char* valueOrName, uint64& outValue);	
};

LV_NS_END

#endif 

