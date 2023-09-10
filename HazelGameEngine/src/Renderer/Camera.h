#pragma once

#include <glm/glm.hpp>

namespace Hazel
{
	class Camera
	{
	public :
		Camera() = default;
		Camera(const glm::mat4& projection) :
			m_ProjectionMatrix(projection) {}

		virtual ~Camera() = default;

		glm::mat4& GetProjection() { return m_ProjectionMatrix; }
		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
	protected :
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.f);
	};
}


