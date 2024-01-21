#pragma once

#include <glm/glm.hpp>
#include "Hazel/Core/Serialization/SerializeTarget.h"

class Serializer;

namespace Hazel
{
	class Camera : public SerializeTarget
	{
	public :
		Camera() = default;
		Camera(const glm::mat4& projection) :
			m_ProjectionMatrix(projection) {}
		virtual ~Camera() = default;

		virtual void Serialize(Serializer* serializer);
		virtual void Deserialize(Serializer* serializer);

		glm::mat4& GetProjection() { return m_ProjectionMatrix; }
		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
	protected :

		virtual void serializeData(Serializer* serializer);
		virtual void deserializeData(Serializer* serializer);

		glm::mat4 m_ProjectionMatrix = glm::mat4(1.f);
	};
}


