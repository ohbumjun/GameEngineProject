#pragma once
#ifndef __LV_LAYER_COMPONENT_H__
#define __LV_LAYER_COMPONENT_H__

#include "engine/component/LvComponent.h"
#include "engine/attribute/LvSerializeVersionAttribute.h"
#include "engine/LvLayerMask.h"

LV_NS_ENGINE_BEGIN

class LV_API 
	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Layer"))
	LV_ATTRIBUTE(LvHideInInspectorAttribute())
	LV_ATTRIBUTE(LvRequireComponentAttribute())
	LV_ATTRIBUTE(LvSerializeVersionAttribute(1))
	LvLayerComponent : public LvComponent
{
public:
	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Layer Mask"))
	LvLayerMask layerMask = LvLayerMask::DEFAULT;

	void Serialize(LvArchive& archive) override;
		
	void Deserialize(LvArchive& archive) override;

};

using LayerComponent = LvLayerComponent;

LV_NS_ENGINE_END

#endif