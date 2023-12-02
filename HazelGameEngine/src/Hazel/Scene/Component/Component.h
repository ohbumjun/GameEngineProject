#pragma once

#include "Hazel/Core/Reflection/TypeId.h"
#include "Hazel/Core/Serialization/SerializeTarget.h"

namespace Hazel
{
	class Component : public SerializeTarget
	{
	public:
		virtual void Serialize(Serializer& serializer) {}
		virtual void Deserialize(Serializer& serializer) {}

		const TypeId GetType() const;
	};
}