#pragma once

#include "Hazel/Scene/Component/Component.h"


namespace Hazel
{
	/*
	Rigid Body 2D 를 둘러싸고 있는 Collier Box2D 
	ex) Rigid Body 2D 가 가로, 세로 1 의 길이라면
	      그에 대응되는 BoxCollider2D 는 extend 가 0.5, 0.5
	*/

	class BoxCollider2DComponent : public Component
	{
		friend class Scene;
	public :
		BoxCollider2DComponent();
		BoxCollider2DComponent(const BoxCollider2DComponent&);

		virtual void Serialize(Serializer* serializer) override;
		virtual void Deserialize(Serializer* serializer) override;

		inline glm::vec2 GetOffset() { return m_Offset; }
		inline glm::vec2& GetOffsetRef() { return m_Offset; }

		inline glm::vec2 GetSize() { return m_Size; }
		inline glm::vec2& GetSizeRef() { return m_Size; }

		inline float GetDensity() { return m_Density; }
		inline float& GetDensityRef() { return m_Density; }

		inline float GetFriction() { return m_Friction; }
		inline float& GetFrictionRef() { return m_Friction; }

		inline float GetRestitution() { return m_Restitution; }
		inline float& GetRestitutionRef() { return m_Restitution; }

		inline float GetRestitutionThreshold() { return m_RestitutionThreshold; }
		inline float& GetRestitutionThresholdRef() { return m_RestitutionThreshold; }

		virtual const TypeId GetType() const
		{
			return Reflection::GetTypeID<BoxCollider2DComponent>();
		}

	private :
		glm::vec2 m_Offset = { 0.0f, 0.0f };

		/*
		Half Extent 와 같은 개념
		*/
		glm::vec2 m_Size = { 0.5f, 0.5f };

		// TODO(Yan): move into physics material in the future maybe
		float m_Density = 1.0f;
		float m_Friction = 0.5f;

		/*
		* Bouncy ness (얼마나 bouncy 하는지)
		*/
		float m_Restitution = 1.0f;

		/*
		* ex) Box 떨어뜨리면 여러번 통통 튀는데
		* 점점 낮게 통통 뛰게 된다.
		*/
		float m_RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* m_RuntimeFixture = nullptr;

	};

}
