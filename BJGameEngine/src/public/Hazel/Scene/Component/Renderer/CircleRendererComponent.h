#pragma once


#include "Hazel/Scene/Component/Component.h"

class Serializer;

namespace Hazel
{
class HAZEL_API CircleRendererComponent : public Component
	{
		friend class Scene;
	public :
		CircleRendererComponent();
		CircleRendererComponent(const CircleRendererComponent&);

		virtual void Serialize(Serializer* serializer);
		virtual void Deserialize(Serializer* serializer);

		inline const glm::vec4& GetColor() const { return m_Color; }
		inline const float GetThickNess() const { return m_Thickness; }
		inline const float GetFade() const { return m_Fade; }

		inline glm::vec4& GetColorRef()  { return m_Color; }
		inline float& GetThickNessRef()  { return m_Thickness; }
		inline float& GetFadeRef()  { return m_Fade; }

		virtual const TypeId GetType() const;
	private :
		glm::vec4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float m_Thickness = 1.0f;
		float m_Fade = 0.005f;
	};

}

