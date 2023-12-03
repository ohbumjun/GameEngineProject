#pragma once

#include "Hazel/Core/Reflection/TypeId.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "Hazel/Core/Serialization/SerializeTarget.h"

class Serializer;

namespace Hazel
{
	class Component : public SerializeTarget
	{
	public:
		Component();
		virtual void Serialize(Serializer* serializer) override {}
		virtual void Deserialize(Serializer* serializer) override {}

		virtual const TypeId GetType() const = 0
		{
			return Reflection::GetTypeID<Component>();
		}
	};
}