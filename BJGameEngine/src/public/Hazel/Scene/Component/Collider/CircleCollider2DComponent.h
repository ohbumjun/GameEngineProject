#pragma once

#include "Hazel/Scene/Component/Component.h"

namespace Hazel
{
class HAZEL_API CircleCollider2DComponent : public Component
	{
		friend class Scene;
	public :
		CircleCollider2DComponent();
		CircleCollider2DComponent(const CircleCollider2DComponent&);

		virtual void Serialize(Serializer* serializer) override;
		virtual void Deserialize(Serializer* serializer) override;

		inline const glm::vec2& GetOffset() const { return m_Offset; }
		inline glm::vec2& GetOffsetRef() { return m_Offset; }
		
		inline float GetRadius() const { return m_Radius; }
		inline float& GetRadiusRef() { return m_Radius; }
		
		inline float GetDensity() const { return m_Density; }
		inline float& GetDensityRef() { return m_Density; }
		
		inline float GetFriction() const { return m_Friction; }
		inline float& GetFrictionRef() { return m_Friction; }
		
		inline float GetRestitution() const { return m_Restitution; }
		inline float& GetRestitutionRef() { return m_Restitution; }

		inline float GetRestitutionThreshold() const { return m_RestitutionThreshold; }
		inline float& GetRestitutionThresholdRef() { return m_RestitutionThreshold; }

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