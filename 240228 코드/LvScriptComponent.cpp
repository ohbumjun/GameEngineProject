#include "LvPrecompiled.h"
#include "engine/component/LvScriptComponent.h"

#include "system/LvReflection.h"
#include "engine/attribute/LvInspectorAttribute.h"
#include "engine/LvScene.h"
#include "engine/private/LvCoroutineInternal.h"


using namespace Lv;

LV_NS_ENGINE_BEGIN

LV_REFLECTION_REGIST
{
	LvReflection::RegistByName<LvScriptComponent>("LvScriptComponent");
	LvReflection::RegistBase<LvComponent, LvArchivable>();
    LvReflection::RegistBase<LvScriptComponent, LvComponent>();
}

LvScriptComponent::~LvScriptComponent()
{
	LvCoroutineInternal::UnregistAll(this);
}

void LvScriptComponent::Serialize(LvArchive & archive)
{
	archive.Serialize(GetType(), this);
}

void LvScriptComponent::Deserialize(LvArchive & archive)
{
 	archive.Deserialize(GetType(), this);
}

LvCoroutineHandle LvScriptComponent::StartCoroutine(LvCoroutine& coroutine)
{
	LvCoroutineInternal::Regist(&coroutine);
	coroutine.Init();
	return LvCoroutineHandle(coroutine);
}

LvCoroutineHandle LvScriptComponent::registCoroutine(LvCoroutine& coroutine)
{
	LvCoroutineInternal::Regist(this, &coroutine);
	coroutine.Init();
	return LvCoroutineHandle(coroutine);
}

void LvScriptComponent::StopCoroutine(LvCoroutineHandle& handle)
{
	LvCoroutineInternal::Unregist(handle.GetPointer());
}

LvGameEntity LvScriptComponent::GetGameEntity() const
{ 
	return LvGameEntity(scene, entity);
}

LV_NS_ENGINE_END
