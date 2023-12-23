#pragma once

#include "Component.h"


namespace Hazel
{

	struct BoxCollider2DComponent : public Component
	{
		glm::vec2 m_Offset = { 0.0f, 0.0f };
		glm::vec2 m_Size = { 0.5f, 0.5f };

		// TODO(Yan): move into physics material in the future maybe
		float m_Density = 1.0f;
		float m_Friction = 0.5f;
		float m_Restitution = 0.0f;
		float m_RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* m_RuntimeFixture = nullptr;

		BoxCollider2DComponent();
		BoxCollider2DComponent(const BoxCollider2DComponent&);
	

		virtual void Serialize(Serializer* serializer) override;
		virtual void Deserialize(Serializer* serializer) override;
	};

}
