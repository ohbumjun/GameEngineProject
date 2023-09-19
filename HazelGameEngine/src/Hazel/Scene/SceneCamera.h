#pragma once

#include "Renderer/Camera.h"

namespace Hazel
{
	class SceneCamera : public Camera
	{
	public :
		enum class ProjectionType { Projective = 0, Orthographic = 1 };

		SceneCamera();
		SceneCamera(const glm::mat4& projection) : Camera(projection) {}
		~SceneCamera();
		void SetOrthographic(float size, float nearDis, float farDis);
		/*
		* 해당 함수는 Render 이전에 호출해야 한다.
		*/
		void SetViewportSize(uint32_t width, uint32_t height);
		void RecalculateProjection();
	
		float GetOrthoGraphicSize() const { return m_OrthographicSize; }
		void SetOrthoGraphicSize(float size) { m_OrthographicSize = size; }
		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; }
	private :
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_OrthographicSize = 10.f;
		float m_OrthographicNear = -1.f, m_OrthographicFar = 1.f;

		// Viewport 는 Resize 되지 않았는데, Camera 의 Orthographic size 는
		// 변경될 수도 있다. Viewport 와 독립적으로 SceneCamera 를 관리하고 싶으면서
		// 덜 ? projection matrix update 를 진행하기 위해 해당 값을 계속 track 한다.
		float m_AspectRatio = 1.f;
	};

}



