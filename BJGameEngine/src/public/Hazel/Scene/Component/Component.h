#pragma once

#include "Hazel/Core/Object/BaseObject.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "Hazel/Core/Reflection/TypeId.h"
#include "Hazel/Core/Serialization/SerializeTarget.h"
#define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/quarternion.hpp>

class Serializer;

namespace Hazel
{
class HAZEL_API Component : public BaseObject
{
public:
    Component();
    virtual void Serialize(Serializer *serializer) override
    {
    }
    virtual void Deserialize(Serializer *serializer) override
    {
    }

    virtual const TypeId GetType() const = 0
    {
        return Reflection::GetTypeID<Component>();
    }
};

} // namespace Hazel