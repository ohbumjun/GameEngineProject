#pragma once

#include "Hazel/Scene/Component/Component.h"


namespace Hazel
{
	/*
	Rigid Body 2D �� �ѷ��ΰ� �ִ� Collier Box2D 
	ex) Rigid Body 2D �� ����, ���� 1 �� ���̶��
	      �׿� �����Ǵ� BoxCollider2D �� extend �� 0.5, 0.5
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
		Half Extent �� ���� ����
		*/
		glm::vec2 m_Size = { 0.5f, 0.5f };

		// TODO(Yan): move into physics material in the future maybe
		float m_Density = 1.0f;
		float m_Friction = 0.5f;

		/*
		* Bouncy ness (�󸶳� bouncy �ϴ���)
		*/
		float m_Restitution = 1.0f;

		/*
		* ex) Box ����߸��� ������ ���� Ƣ�µ�
		* ���� ���� ���� �ٰ� �ȴ�.
		*/
		float m_RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* m_RuntimeFixture = nullptr;

	};

}
