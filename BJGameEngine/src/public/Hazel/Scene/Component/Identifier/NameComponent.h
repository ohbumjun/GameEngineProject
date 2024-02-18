#pragma once


#include "Hazel/Scene/Component/Component.h"

namespace Hazel
{
class HAZEL_API NameComponent : public Component
	{
		friend class Scene;
	public:
		NameComponent();
		NameComponent(const NameComponent& other) :
			name(other.name)
		{
			NameComponent();
		}
		NameComponent(const std::string& name) :
			name(name) 
		{
			NameComponent();
		}

		operator const std::string& () const { return name; }
		operator std::string& () { return name; }
		inline const std::string& GetName() { return name; }

		virtual void Serialize(Serializer* serializer);
		virtual void Deserialize(Serializer* serializer);
	
		virtual const TypeId GetType() const;
	private:
		std::string name = "";
	};
}


