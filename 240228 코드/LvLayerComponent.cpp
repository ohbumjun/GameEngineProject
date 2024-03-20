#include "LvPrecompiled.h"
#include "engine/component/LvLayerComponent.h"

#include "system/LvReflection.h"

using namespace Lv;

LV_NS_ENGINE_BEGIN

void LvLayerComponent::Serialize(LvArchive& archive)
{
	archive.WriteStartObject( _type , this);
	LV_WRITE(archive, enabled);
	LV_WRITE(archive, layerMask);
	archive.WriteEndObject();
}

void LvLayerComponent::Deserialize(LvArchive& archive)
{
	archive.ReadStartObject( _type , this);
	LV_READ(archive, enabled);
	LV_READ(archive, layerMask);
	archive.ReadEndObject();
}


LV_NS_ENGINE_END

