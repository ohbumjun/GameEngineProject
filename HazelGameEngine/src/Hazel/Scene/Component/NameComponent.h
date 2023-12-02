#pragma once

#include "Component.h"
#include "Hazel/Core/Serialization/Serializer.h"

namespace Hazel
{
	class NameComponent : public Component
	{
		friend class Scene;
	public:
		NameComponent() = default;
		NameComponent(const NameComponent& other) :
			name(other.name) {};
		NameComponent(const std::string& name) :
			name(name) {}

		operator const std::string& () const { return name; }
		operator std::string& () { return name; }
		const std::string& GetName() { return name; }

		virtual void Serialize(Serializer* serializer);
		virtual void Deserialize(Serializer* serializer);
	
		virtual const TypeId GetType() const;
	private:
		std::string name = "";
	};
}


