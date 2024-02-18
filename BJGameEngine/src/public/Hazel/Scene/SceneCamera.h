#pragma once

#include "Renderer/Camera/Camera.h"

class Serializer;

namespace Hazel
{
class HAZEL_API SceneCamera : public Camera
	{
	public :
		enum class ProjectionType { Projective = 0, Orthographic = 1 };

		SceneCamera();
		SceneCamera(const glm::mat4& projection) : Camera(projection) {}
		~SceneCamera();

		virtual void Serialize(Serializer* serializer);
		virtual void Deserialize(Serializer* serializer);

		void SetOrthographic(float size, float nearDis, float farDis);
		void SetPerspective(float fov, float nearDis, float farDis);

		/*
		* 해당 함수는 Render 이전에 호출해야 한다.
		*/
		void SetViewportSize(uint32_t width, uint32_t height);
		void RecalculateProjection();
	
		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type;  RecalculateProjection();}

		// ortho 와 projective 의 near, far 은 구분되어야 한다.
		// ortho 는 near, far 아 -1, 1 정도이지만
		// projective 는 near, far 이 0 , 1000 이렇게 될 수 있기 때문이다.
		float GetOrthoGraphicSize() const { return m_OrthographicSize; }
		void SetOrthoGraphicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		float GetOrthographicNear()	const { return m_OrthographicNear; }
		float GetOrthographicFar()		const { return m_OrthographicFar; }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection();}
		void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection();}

		float GetPerspectiveFov() const { return m_PerspectiveFov; }
		void SetPerspectiveFov(float fov) { m_PerspectiveFov = fov; RecalculateProjection(); }
		float GetPerspectiveNear()	const { return m_PerspectiveNear; }
		float GetPerspectiveFar()		const { return m_PerspectiveFar; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }
	private :
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_OrthographicSize = 10.f;
		float m_OrthographicNear = -1.f, m_OrthographicFar = 1.f;

		float m_PerspectiveFov = glm::radians(45.f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.f;

		// Viewport 는 Resize 되지 않았는데, Camera 의 Orthographic size 는
		// 변경될 수도 있다. Viewport 와 독립적으로 SceneCamera 를 관리하고 싶으면서
		// 덜 ? projection matrix update 를 진행하기 위해 해당 값을 계속 track 한다.
		float m_AspectRatio = 1.f;
	};

}



