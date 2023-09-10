#pragma once

#include <glm/glm.hpp>

namespace Hazel
{
	class Camera
	{
	public :
		Camera(const glm::mat4& projection) :
			m_ProjectionMatrix(projection) {}
	private :
		glm::mat4 m_ProjectionMatrix;
	};
}


