#include "system/util/LvStringUtil.h"

#include "system/LvString.h"
#include "system/LvSystemContext.h"

LV_NS_BEGIN

int LvStringUtil::GetExtraLengthForUTF8(const char* src)
{
	if (nullptr == src) return 0;

	int added = 0;
	size_t srcLen = strlen(src);
	for (size_t i = 0; i < srcLen; ++i)
	{
		if (static_cast<int>(src[i]) < 0)
		{
			int charI = static_cast<uint>(src[i]) % 0x100;
			int addedBytes = 0;

			//https://en.wikipedia.org/wiki/UTF-8#Description
			//flags is Byte1 checker
			int flags = ((charI & 0x40) >> 4) + ((charI & 0x20) >> 4) + ((charI & 0x10) >> 4);

			if (flags & 0b0100)
			{
				if (flags & 0b0010)
				{
					if (flags & 1)
					{
						addedBytes = 3;
					}
					else
					{
						addedBytes = 2;
					}
				}
				else
				{
					addedBytes = 1;
				}
			}

			added += addedBytes;
			i += addedBytes;
		}
	}
	return added;
}

bool LvStringUtil::IsUTF8(const char* value)
{ 
	for (size_t i = 0; i < strlen(value); ++i)
	{
		int num;
		char temp = value[i];
		if ((temp & 0x80) == 0x00)
		{
			// U+0000 to U+007F 
			num = 1;
		}
		else if ((temp & 0xE0) == 0xC0)
		{
			// U+0080 to U+07FF 
			num = 2;
		}
		else if ((temp & 0xF0) == 0xE0)
		{
			// U+0800 to U+FFFF 
			num = 3;
		}
		else if ((temp & 0xF8) == 0xF0)
		{
			// U+10000 to U+10FFFF 
			num = 4;
		}
		else
		{
			return false;
		}

		for (int j = 1; j < num; ++j)
		{
			char t = value[++i];
			if ((t & 0xC0) != 0x80)
			{
				return false;
			}
		}
	}
	return true;
}

LvString LvStringUtil::ToString(int8 value)
{
	LvString ret;
	ret.FormatSelf("%d", value);
	return ret;
}

LvString LvStringUtil::ToString(int16 value)
{
	LvString ret;
	ret.FormatSelf("%d", value);
	return ret;
}

LvString LvStringUtil::ToString(int32 value)
{
	LvString ret;
	ret.FormatSelf("%d", value);
	return ret;
}

LvString LvStringUtil::ToString(int64 value)
{
	LvString ret;
	ret.FormatSelf("%lld", value);
	return ret;
}

LvString LvStringUtil::ToString(uint8 value)
{
	LvString ret;
	ret.FormatSelf("%u", value);
	return ret;
}

LvString LvStringUtil::ToString(uint16 value)
{
	LvString ret;
	ret.FormatSelf("%u", value);
	return ret;
}

LvString LvStringUtil::ToString(uint32 value)
{
	LvString ret;
	ret.FormatSelf("%u", value);
	return ret;
}

LvString LvStringUtil::ToString(uint64 value)
{
	LvString ret;
	ret.FormatSelf("%llu", value);
	return ret;
}

LvString LvStringUtil::ToString(float value)
{
	LvString ret;
	ret.FormatSelf("%f", value);
	return ret;
}

LvString LvStringUtil::ToString(double value)
{
	LvString ret;
	ret.FormatSelf("%lf", value);
	return ret;
}

int8 LvStringUtil::ToInt8(const char * value)
{
	char* error = nullptr;
	int8 result = (int8)strtol(value, &error, 10);
	if (error == nullptr) LV_THROW("strtoul error, %s", error);
	return result;
}

int16 LvStringUtil::ToInt16(const char * value)
{
	char* error = nullptr;
	int16 result = (int16)strtol(value, &error, 10);
	if (error == nullptr) LV_THROW("strtoul error, %s", error);
	return result;
}

int32 LvStringUtil::ToInt32(const char * value)
{
	char* error = nullptr;
	int32 result = strtol(value, &error, 10);
	if (error == nullptr) LV_THROW("strtoul error, %s", error);
	return result;
}

int64 LvStringUtil::ToInt64(const char * value)
{
	char* error = nullptr;
	int64 result = strtoll(value, &error, 10);
	if (error == nullptr) LV_THROW("strtoul error, %s", error);
	return result;
}

uint8 LvStringUtil::ToUint8(const char * value)
{
	char* error = nullptr;
	uint8 result = (uint8)strtoul(value, &error, 10);
	if (error == nullptr) LV_THROW("strtoul error, %s", error);
	return result;
}

uint16 LvStringUtil::ToUint16(const char * value)
{
	char* error = nullptr;
	uint16 result = (uint16)strtoul(value, &error, 10);
	if (error == nullptr) LV_THROW("strtoul error, %s", error);
	return result;
}

uint32 LvStringUtil::ToUint32(const char * value)
{
	char* error = nullptr;
	uint32 result = strtoul(value, &error, 10);
	if (error == nullptr) LV_THROW("strtoul error, %s", error);
	return result;
}

uint64 LvStringUtil::ToUint64(const char * value)
{
	char* error = nullptr;
	uint64 result = strtoull(value, &error, 10);
	if (error == nullptr) LV_THROW("strtoul error, %s", error);
	return result;
}

float LvStringUtil::ToFloat(const char * value)
{
	char* end;
	float f = 0.0;
	f = strtof(value, &end);
	return f;
}

double LvStringUtil::ToDouble(const char * value)
{
	char* end;
	double d = 0.0;
	d = strtod(value, &end);
	return d;
}

LV_NS_END
