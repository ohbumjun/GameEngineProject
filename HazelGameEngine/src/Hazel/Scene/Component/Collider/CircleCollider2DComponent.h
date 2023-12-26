#pragma once

#include "Hazel/Scene/Component/Component.h"

namespace Hazel
{
	class CircleCollider2DComponent : public Component
	{
		friend class Scene;
	public :
		CircleCollider2DComponent();
		CircleCollider2DComponent(const CircleCollider2DComponent&);

		virtual void Serialize(Serializer* serializer) override;
		virtual void Deserialize(Serializer* serializer) override;

		float GetRadius() const { return m_Radius; }
		const glm::vec2& GetOffset() const { return m_Offset; }
		float GetDensity() const { return m_Density; }
		float GetFriction() const { return m_Friction; }
		float GetRestitution() const { return m_Restitution; }
		float GetRestitutionThreshold() const { return m_RestitutionThreshold; }

		virtual const TypeId GetType() const;
	private :
		glm::vec2 m_Offset = { 0.0f, 0.0f };
		float m_Radius = 0.5f;

		// TODO(Yan): move into physics material in the future maybe
		float m_Density = 1.0f;
		float m_Friction = 0.5f;
		float m_Restitution = 0.0f;
		float m_RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* m_RuntimeFixture = nullptr;
};

}