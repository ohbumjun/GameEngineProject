#pragma once

#include "Component.h"

namespace Hazel
{
	class Rigidbody2DComponent : public Component
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType m_Type = BodyType::Static;
		bool m_FixedRotation = false;

		// Storage for runtime
		void* m_RuntimeBody = nullptr;

		Rigidbody2DComponent();
		Rigidbody2DComponent(const Rigidbody2DComponent&);
	
		virtual void Serialize(Serializer* serializer) override;
		virtual void Deserialize(Serializer* serializer) override;
	};


}
