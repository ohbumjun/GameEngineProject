#pragma once

#include "Camera.h"
#include "Hazel/Utils/TimeStep.h"
#include "Hazel/Event/Event.h"
#include "Hazel/Event/MouseEvent.h"

#include <glm/glm.hpp>

namespace Hazel {

	class HAZEL_API EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		// Getter
		inline float GetDistance() const { return m_Distance; }

		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		/*
		World 상에서 원점 기준으로
		카메라가 바라보는 방향 벡터값을 리턴한다.
		*/
		glm::vec3 GetForwardDirection() const;
		inline const glm::vec3& GetPosition() const { return m_Position; }

		/*
		단순히 x,y 회전값이 얼마인지
		*/
		glm::quat GetOrientation() const;

		inline float GetPerspectiveFov() const { return m_FOV; }
		inline float GetPerspectiveNear() const { return m_NearClip; }
		inline float GetPerspectiveFar() const { return m_FarClip; }

		inline float GetPitch() const { return m_Pitch; }
		inline float GetYaw() const { return m_Yaw; }

		// Setter
		inline void SetDistance(float distance) { m_Distance = distance; }
		inline void SetViewportSize(float width, float height) 
		{ 
			m_ViewportWidth = width; m_ViewportHeight = height; updateProjection(); 
		}
		inline void SetPerspectiveFov(float FOV)  
		{  
			m_FOV = FOV;  updateProjection();
		}
		inline void SetPerspectiveNearClip(float Near)
		{
			m_NearClip = Near;  updateProjection();
		}
		inline void SetPerspectiveFarClip(float Far)
		{
			m_FarClip = Far;  updateProjection();
		}
	private:
		void updateProjection();
		void updateView();

		bool onMouseScroll(MouseScrolledEvent& e);

		/*
		정리
		1) MousePan, MouseZoom 은 간단하게 말하면
		Camera 의 Transition 을 이동시키는 것 
		2) MouseRotate 는 
		Camera 의 Rotation 을 변경시키는 것

		각각을 Transition, Rotate 로 고려해서
		World 변환 행렬 S,R,T 를 만드는 것이다.
		*/
		// 마우스를 이동시키는 함수
		void mousePan(const glm::vec2& delta);
		// 마우스를 회전시키는 함수
		void mouseRotate(const glm::vec2& delta);
		// 마우스 줌인을 해주는 함수
		void mouseZoom(float delta);

		glm::vec3 calculatePosition() const;

		std::pair<float, float> panSpeed() const;
		float rotationSpeed() const;

		/*
		Mouse Scroll 하면서 Camera 의 Zoom Speed 를
		조절해주는 함수

		z = 0 에 가까울 수록, zoom speed 는 줄어들어서
		천천히 가까워지고

		z = 0 으로부터 멀수록, zoom speed 는 빨라져서
		빨리 가까워진다.
		*/
		float ZoomSpeed() const;


		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		glm::mat4 m_ViewMatrix;
		
		/*
		World 상에서 Camera 의 Pos 라고 생각하면 된다.
		Translation 정보
		*/
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };

		/*
		Focal point 라는 것의 의미는, 해당 지점
		beyond 로 넘어갈 수 없다는 것을 의미한다.

		쉽게 말해 Camera 가 Focusing 하는 중앙 지점이라고
		생각해도 된다.

		MousePan 함수에서 Focal Point 를 이동시킨다.
		Camera 의 특정 Position  을 이동시키는 개념이 아니다.
		*/
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		/*
		Scene 의 원점 (0,0,0) 을 focusing 하면서
		동시에 조금 뒤에서 해당 원점을 바라보게 하는 것이다.

		z 가 1보다 작지 않게 조절한다.
		즉, 너무 앞으로 가서 물체들 뒤로 넘어가는 것을
		방지하기 위한 것 같기도 하다.
		*/
		float m_Distance = 10.0f;

		/*
		Pitch : 카메라 방향의 x 축 (위, 아래) 회전 각도
		Yaw  : 카메라 방향의 y 축 (왼, 오) 회전 각도
		z 축으로의 회전은 방지하는 것으로 보인다.

		OPENGL 오른손 좌표계에서는
		각 축을 기준으로, 반시계 방향이
		양수 회전값이라고 생각하면 된다.
		*/
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};

}