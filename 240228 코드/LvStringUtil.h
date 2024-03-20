#pragma once

#ifndef __LV_PRIMITIVE_TO_STRING_H__
#define __LV_PRIMITIVE_TO_STRING_H__

#include "LvPrecompiled.h"
#include "system/LvLog.h"
#include "system/LvString.h"
#include "system/LvReflection.h"
#include "system/LvEnum.h"

LV_NS_BEGIN

struct LV_API LvStringUtil
{
	static bool IsUTF8(const char* value);

	/**
	 * @brief platform os system path에서 char(utf8 nfc) path로 변환합니다.
	 * @param dest 변경 이후 utf8 경로
	 * @param src 변경 이전 시스템 경로 (windows: wchar_t, unix: utf8(nfd))
	 * @file #include "system/util/LvStringUtil.h"
	 */
	static void SystemToUTF8(char* dest, const void* src);

	/**
	 * @brief char(utf8) path에서 os system path로 변환합니다.
	 * @param dest 변경 이후 경로 (windows: wchar_t, mac: NSString)
	 * @param src 변경 이전 경로
	 * @param safe utf-8이 아닌 ANSI type까지 처리 (only for window)
	 * @file #include "system/util/LvStringUtil.h"
	 */
	static void UTF8ToSystem(void* dest, const char* src, bool safe = false);
	
	/**
	 * @brief char(utf8(nfc)) 문자열 가변 byte로 인해 추가된 byte수를 계산합니다.
	 * @param src utf8 경로
	 * @return 1byte보다 긴 가변 bytes 수
	 * @file #include "system/util/LvStringUtil.h"
	 */
	static int GetExtraLengthForUTF8(const char* src);

	template< typename T, std::enable_if_t<std::is_enum<T>::value>* = nullptr>
	static LvString ToString(T value)
	{
		const auto info = LvReflection::GetTypeInfo<T>();
		if (nullptr != info)
		{
			if (info->isEnum)
			{
				return LvEnum::GetName(value);
			}
			else
			{
				LV_LOG(warning, "Type is not enum.");
			}
		}
		else
		{
			LV_LOG(warning, "Type is not registered in reflection.");
		}
		return ToString(static_cast<int32>(value));
	}

	static LvString ToString(int8 value);
	static LvString ToString(int16 value);
	static LvString ToString(int32 value);
	static LvString ToString(int64 value);
	
	static LvString ToString(uint8 value);
	static LvString ToString(uint16 value);
	static LvString ToString(uint32 value);
	static LvString ToString(uint64 value);
	
	static LvString ToString(float value);
	static LvString ToString(double value);
	
	static int8		ToInt8(const char* value);
	static int16	ToInt16(const char* value);
	static int32	ToInt32(const char* value);
	static int64	ToInt64(const char* value);
	
	static uint8	ToUint8(const char* value);
	static uint16	ToUint16(const char* value);
	static uint32	ToUint32(const char* value);
	static uint64	ToUint64(const char* value);
	
	static float ToFloat(const char* value);
	static double ToDouble(const char* value);

};

#define TO_C_STR(v) Lv::LvStringUtil::ToString(v).c_str();

LV_NS_END

#endif
