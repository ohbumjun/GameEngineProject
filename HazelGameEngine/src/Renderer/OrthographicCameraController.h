#pragma once

#include "Renderer/OrthographicCamera.h"
#include "Hazel/Core/TimeStep.h"

#include "Hazel/Event/ApplicationEvent.h"
#include "Hazel/Event/MouseEvent.h"

namespace Hazel
{
	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};
	class OrthographicCameraController
	{
	public :
		OrthographicCameraController(float aspectRatio, float rotation = false); // aspect ratio * 2 units

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		const OrthographicCamera& GetCamera() const { return m_Camera; }
		OrthographicCamera& GetCamera() { return m_Camera; }
	
		void SetZoomLevel(float level) { m_ZoomLevel = level; }
		float GetZoomLevel() { return m_ZoomLevel; }
		const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }
	private :
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
	private :
		float m_AspectRatio;
		float m_ZoomLevel = 1.f;

		OrthographicCamera m_Camera;
		OrthographicCameraBounds m_Bounds;
		bool m_Rotation;
		float m_CameraRot = 0.f;
		glm::vec3 m_CameraPos = { 0.f, 0.f, 0.f };
		float m_CameraRotSpeed = 10.f;
		float m_CameraMoveSpeed = 1.0f;
	};

}


