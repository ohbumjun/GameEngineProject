#include "hzpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), 
		m_ViewMatrix(1.0f) // identity matrix
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void OrthographicCamera::SetPosition(const glm::vec3& position)
	{
		m_Position = position; 
		RecalculateViewMatrix();
	}
	void OrthographicCamera::SetRotation(float Rot)
	{
		m_Rotation = Rot; 
		RecalculateViewMatrix();
	}
	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		HZ_PROFILE_FUNCTION();

		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void OrthographicCamera::RecalculateViewMatrix()
	{
		HZ_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.f), m_Position);
		
		// rotate z axis;
		transform *= glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));
	
		// inverse
		m_ViewMatrix = glm::inverse(transform);
	
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}