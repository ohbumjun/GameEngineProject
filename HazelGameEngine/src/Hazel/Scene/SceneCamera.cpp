#include "hzpch.h"
#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>

Hazel::SceneCamera::SceneCamera()
{
	RecalculateProjection();
}

Hazel::SceneCamera::~SceneCamera()
{
}

void Hazel::SceneCamera::SetOrthographic(float size, float nearDis, float farDis)
{
	m_ProjectionType = ProjectionType::Orthographic;

	m_OrthographicSize = size;
	m_OrthographicNear = nearDis;
	m_OrthographicFar = farDis;

	RecalculateProjection();
}

void Hazel::SceneCamera::SetPerspective(float fov, float nearDis, float farDis)
{
	m_ProjectionType = ProjectionType::Projective;
	m_PerspectiveFov = fov;
	m_PerspectiveNear = nearDis;
	m_PerspectiveFar = farDis;

	RecalculateProjection();
}

void Hazel::SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
{
	m_AspectRatio = (float)width / (float)height;

	RecalculateProjection();
}

void Hazel::SceneCamera::RecalculateProjection()
{	
	if (m_ProjectionType == ProjectionType::Orthographic)
	{
		// height : orthographic size
		// width  : orthographic size (viewport size) * aspect ratio
		float orthoLeft = m_OrthographicSize * m_AspectRatio * -0.5f;
		float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoTop = m_OrthographicSize * 0.5f;
		float orthoBottom = m_OrthographicSize * -0.5f;

		/*
		orthoLeft, orthoRight, orthoTop, orthoBottom: These parameters define the boundaries of the orthographic projection in world space.
		The left, right, top, and bottom values specify the extents of the view volume along the X and Y axes.
		Objects within this volume will be visible in the rendered scene.

		즉, 해당 값들이 작다면, 보여지는 공간이 작다는 것이고, 보여지는 대상이 커보인다는 것이다.
		반면, 해당 값들이 크다면, 보여지는 공간이 넓다는 의미이고, 보여지는 대상이 작아보인다는 것이다.
		*/
		m_ProjectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoTop, orthoBottom, m_OrthographicNear, m_OrthographicFar);
	}
	else
	{
		m_ProjectionMatrix = glm::perspective(m_PerspectiveFov, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
	}
}
