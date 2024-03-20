#include "engine/LvLayerMask.h"
#include "system/LvEnum.h"
#include <cmath>

	
LV_NS_ENGINE_BEGIN

static LvHashtable<LvString, uint32> s_layerMaskMapping;

void RegistLayerMaskMappingName(const LvString& name, LvLayerMask layerMask)
{
	if (!s_layerMaskMapping.ContainsKey(name))
	{
		s_layerMaskMapping.AddInitialized(name);
	}

	
	s_layerMaskMapping[name] = static_cast<uint32>(std::round(std::log2(static_cast<uint32>(layerMask))));
}

void UnregistLayerMaskMappingName(const LvString& name)
{
	if (s_layerMaskMapping.ContainsKey(name))
	{
		s_layerMaskMapping.Remove(name);
	}
}

void UnregistAllMappingName() 
{
	s_layerMaskMapping.Clear();
}

const LvString& GetLayerMaskMappingName(LvLayerMask layerMask) 
{
	for (auto& pair : s_layerMaskMapping)
	{
		if (pair.value == static_cast<uint32>(layerMask))
		{
			return pair.key;
		}
	}

	return Lv::LvEnum::GetName(layerMask);
}

LvLayerMask GetLayerMask(const LvString& name)
{
	if (s_layerMaskMapping.ContainsKey(name)) 
	{
		return static_cast<LvLayerMask>(s_layerMaskMapping[name]);
	}

	return Lv::LvEnum::GetValue<LvLayerMask>(name.c_str());
}

const LvHashtable<LvString, uint32>& GetLayerMaskMapping()
{
	return s_layerMaskMapping;
}

LV_NS_ENGINE_END
