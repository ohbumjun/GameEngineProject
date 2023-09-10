#pragma once

#include <glm/glm.hpp>

namespace Hazel
{
	class Camera
	{
	public :
		Camera(const glm::mat4& projection) :
			m_ProjectionMatrix(projection) {}

		glm::mat4& GetProjection() { return m_ProjectionMatrix; }
		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
	private :
		glm::mat4 m_ProjectionMatrix;
	};
}


