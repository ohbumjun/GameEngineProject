#pragma once

#include "Component.h"

namespace Hazel
{
	/*
	Static
	-  �ƿ� Fixed �� �༮. Gravity �� ���� ������ �ʴ´�.

	Dynamic
	- Gravity �� ���� �޴� ��
	ex) Earth Rotating 
	*/

	class Rigidbody2DComponent : public Component
	{
		friend class Scene;
	public :
		enum class BodyType { Static = 0, Dynamic, Kinematic };

		Rigidbody2DComponent();
		Rigidbody2DComponent(const Rigidbody2DComponent&);
	
		virtual void Serialize(Serializer* serializer) override;
		virtual void Deserialize(Serializer* serializer) override;

		inline BodyType GetBodyType() { return m_Type; }
		inline BodyType& GetBodyTypeRef() { return m_Type; }

		inline bool GetFixedRotation() { return m_FixedRotation; }
		inline bool& GetFixedRotationRef() { return m_FixedRotation; }

		inline void* GetRuntimeBody() const {return m_RuntimeBody;}

		inline void SetBodyType(BodyType type) { m_Type = type; }
	
		virtual const TypeId GetType() const 
		{
			return Reflection::GetTypeID<Rigidbody2DComponent>();
		}
	private:
		BodyType m_Type = BodyType::Static;

		/*
		true : not able to rotate
		*/
		bool m_FixedRotation = false;

		/*
		Storage for runtime
		*/
		void* m_RuntimeBody = nullptr;
	};


}
