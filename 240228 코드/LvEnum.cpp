#include "LvPrecompiled.h"
#include "system/LvEnum.h"

#include "system/LvLog.h"
#include "system/util/LvStringUtil.h"

LV_NS_BEGIN
	bool LvEnum::Contain(LvTypeId enumType, const char* enumName)
{
	const LvTypeInfo* info = LvReflection::GetTypeInfo(enumType);

	if (info)
	{
		for (auto& each : info->enumFields)
		{
			if (enumName == each.name)
			{
				return true;
			}
		}
	}

	return false;
}

const char* LvEnum::GetName(LvTypeId enumType, uint64 enumValue)
{
	LV_ASSERT(LvReflection::HasRegist(enumType), "T should be registed");
	const LvTypeInfo* info = LvReflection::GetTypeInfo(enumType);

	for (const auto& each : info->enumFields)
	{
		if (enumValue == each.value)
		{
			return each.name.c_str();
		}
	}

	return nullptr;
}


uint64 LvEnum::GetValue(LvTypeId enumType, const char* enumName)
{
	LV_ASSERT(LvReflection::HasRegist(enumType), "T should be registed");

	const LvTypeInfo* info = LvReflection::GetTypeInfo(enumType);

	for (auto& each : info->enumFields)
	{
		if (enumName == each.name)
		{
			return each.value;
		}
	}

	LV_CHECK(true, "Not found enum value");
	return 0;
}

bool LvEnum::TryGetValue(LvTypeId enumType, const char* enumName, uint64& outValue)
{
	const LvTypeInfo* info = LvReflection::GetTypeInfo(enumType);

	if (info)
	{
		for (auto& each : info->enumFields)
		{
			if (enumName == each.name)
			{
				outValue = each.value;
				return true;
			}
		}
	}
	return false;
}

bool LvEnum::TryParse(LvTypeId enumType, const char* valueOrName, uint64& outValue)
{
	const LvTypeInfo* info = LvReflection::GetTypeInfo(enumType);

	if (info)
	{
		for (const auto& each : info->enumFields)
		{
			if (each.name == valueOrName)
			{
				outValue = each.value;
				return true;
			}

			if (LvStringUtil::ToString(each.value) == valueOrName)
			{
				outValue = each.value;
				return true;
			}
		}
	}
	return false;
}

LV_NS_END

