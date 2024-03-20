#pragma warning(disable:4251)
#pragma once
#ifndef __LV_LAYER_MASK_H__
#define __LV_LAYER_MASK_H__

#include "LvPrecompiled.h"
#include "system/LvReflection.h"
#include "system/LvHashtable.h"
#include "system/LvString.h"
	
LV_NS_ENGINE_BEGIN
	
enum class LV_REFLECT LvLayerMask : uint32
{
	NONE = 0,
	DEFAULT = (1 << 0),
	TRANSPARENTFX = (1 << 1),
	UI = (1 << 2),
	POST_EFFECT = (1 << 3),
	ICON = (1 << 4),
	RESERVED = (1 << 5),

	USER_LAYER_6 = (1 << 6),
	USER_LAYER_7 = (1 << 7),
	USER_LAYER_8 = (1 << 8),
	USER_LAYER_9 = (1 << 9),
	USER_LAYER_10 = (1 << 10),
	USER_LAYER_11 = (1 << 11),
	USER_LAYER_12 = (1 << 12),
	USER_LAYER_13 = (1 << 13),
	USER_LAYER_14 = (1 << 14),
	USER_LAYER_15 = (1 << 15),
	USER_LAYER_16 = (1 << 16),
	USER_LAYER_17 = (1 << 17),
	USER_LAYER_18 = (1 << 18),
	USER_LAYER_19 = (1 << 19),
	USER_LAYER_20 = (1 << 20),
	USER_LAYER_21 = (1 << 21),
	USER_LAYER_22 = (1 << 22),
	USER_LAYER_23 = (1 << 23),
	USER_LAYER_24 = (1 << 24),
	USER_LAYER_25 = (1 << 25),
	USER_LAYER_26 = (1 << 26),
	USER_LAYER_27 = (1 << 27),
	USER_LAYER_28 = (1 << 28),
	USER_LAYER_29 = (1 << 29),
	USER_LAYER_30 = (1 << 30),
	ALL = UINT32_MAX

};

LV_API void RegistLayerMaskMappingName(const LvString& name, LvLayerMask layerMask);

LV_API void UnregistLayerMaskMappingName(const LvString& name);

LV_API void UnregistAllMappingName();

LV_API const LvString& GetLayerMaskMappingName(LvLayerMask layerMask);

LV_API LvLayerMask GetLayerMask(const LvString& name);

LV_API const LvHashtable<LvString, uint32>& GetLayerMaskMapping();

LV_NS_ENGINE_END

#endif
