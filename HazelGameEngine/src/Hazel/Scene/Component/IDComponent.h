#pragma once

#include "Hazel/Core/ID//UUID.h"
#include "Component.h"

namespace Hazel
{
	class IDComponent : public Component
	{
		friend class Scene;
	public :
		IDComponent();
		IDComponent(const IDComponent&);

		virtual const TypeId GetType() const;
		virtual void Serialize(Serializer* serializer);
		virtual void Deserialize(Serializer* serializer);
	private:
		UUID ID;

	};

}

