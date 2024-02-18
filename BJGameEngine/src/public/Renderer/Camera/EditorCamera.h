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
		World �󿡼� ���� ��������
		ī�޶� �ٶ󺸴� ���� ���Ͱ��� �����Ѵ�.
		*/
		glm::vec3 GetForwardDirection() const;
		inline const glm::vec3& GetPosition() const { return m_Position; }

		/*
		�ܼ��� x,y ȸ������ ������
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
		����
		1) MousePan, MouseZoom �� �����ϰ� ���ϸ�
		Camera �� Transition �� �̵���Ű�� �� 
		2) MouseRotate �� 
		Camera �� Rotation �� �����Ű�� ��

		������ Transition, Rotate �� ����ؼ�
		World ��ȯ ��� S,R,T �� ����� ���̴�.
		*/
		// ���콺�� �̵���Ű�� �Լ�
		void mousePan(const glm::vec2& delta);
		// ���콺�� ȸ����Ű�� �Լ�
		void mouseRotate(const glm::vec2& delta);
		// ���콺 ������ ���ִ� �Լ�
		void mouseZoom(float delta);

		glm::vec3 calculatePosition() const;

		std::pair<float, float> panSpeed() const;
		float rotationSpeed() const;

		/*
		Mouse Scroll �ϸ鼭 Camera �� Zoom Speed ��
		�������ִ� �Լ�

		z = 0 �� ����� ����, zoom speed �� �پ��
		õõ�� ���������

		z = 0 ���κ��� �ּ���, zoom speed �� ��������
		���� ���������.
		*/
		float ZoomSpeed() const;


		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		glm::mat4 m_ViewMatrix;
		
		/*
		World �󿡼� Camera �� Pos ��� �����ϸ� �ȴ�.
		Translation ����
		*/
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };

		/*
		Focal point ��� ���� �ǹ̴�, �ش� ����
		beyond �� �Ѿ �� ���ٴ� ���� �ǹ��Ѵ�.

		���� ���� Camera �� Focusing �ϴ� �߾� �����̶��
		�����ص� �ȴ�.

		MousePan �Լ����� Focal Point �� �̵���Ų��.
		Camera �� Ư�� Position  �� �̵���Ű�� ������ �ƴϴ�.
		*/
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		/*
		Scene �� ���� (0,0,0) �� focusing �ϸ鼭
		���ÿ� ���� �ڿ��� �ش� ������ �ٶ󺸰� �ϴ� ���̴�.

		z �� 1���� ���� �ʰ� �����Ѵ�.
		��, �ʹ� ������ ���� ��ü�� �ڷ� �Ѿ�� ����
		�����ϱ� ���� �� ���⵵ �ϴ�.
		*/
		float m_Distance = 10.0f;

		/*
		Pitch : ī�޶� ������ x �� (��, �Ʒ�) ȸ�� ����
		Yaw  : ī�޶� ������ y �� (��, ��) ȸ�� ����
		z �������� ȸ���� �����ϴ� ������ ���δ�.

		OPENGL ������ ��ǥ�迡����
		�� ���� ��������, �ݽð� ������
		��� ȸ�����̶�� �����ϸ� �ȴ�.
		*/
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};

}