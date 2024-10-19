#include "Hazel/Scene/Component/NativeScriptComponent.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "Hazel/Core/Serialization/Serializer.h"
#include "hzpch.h"

namespace Hazel
{

NativeScriptComponent::NativeScriptComponent()
{
    Reflection::RegistType<NativeScriptComponent>();
}

void NativeScriptComponent::Serialize(Serializer *serializer)
{
}

void NativeScriptComponent::Deserialize(Serializer *serializer)
{
}

const TypeId NativeScriptComponent::GetType() const
{
    return Reflection::GetTypeID<NativeScriptComponent>();
}

} // namespace Hazel