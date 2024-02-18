#pragma once

#include <glm/glm.hpp>

namespace Hazel
{
	// 2d camera
class HAZEL_API OrthographicCamera
	{
	public :
		// 가로, 세로 폭을 지정하는 것이다.
		// ex) left, right : -1.f, 1.f --> -2.f, 2.f 가 된다는 것은 카메라가 보는 범위가 -2.f ~ 2.f 가 된다는 것
		//      즉, 기존의 보이던 물체의 크기가 반으로 줄어든다는 것을 의미한다.
		OrthographicCamera(float left, float right, float bottom, float top);

		float GetRotation() { return m_Rotation; }
		const glm::vec3& GetPosition() const { return m_Position; }
		
		void SetPosition(const glm::vec3& position);
		void SetRotation(float Rot);
		void SetProjection(float left, float right, float bottom, float top);


		const glm::mat4& GetProjectionMatrix() { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() { return m_ViewProjectionMatrix; }
	private :
		void RecalculateViewMatrix();

	private :
		// inverse of transformation matrix of camera
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
		float m_Rotation = 0.f;
	};
}



