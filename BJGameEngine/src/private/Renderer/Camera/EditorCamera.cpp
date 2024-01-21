#include "hzpch.h"
#include "EditorCamera.h"

#include "Hazel/Input/Input.h"
#include "Hazel/Input/KeyCodes.h"
#include "Hazel/Input/MouseButtonCodes.h"


#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Hazel {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		updateView();
	}

	void EditorCamera::updateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::updateView()
	{ 
		// m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
		m_Position = calculatePosition();

		glm::quat orientation = GetOrientation();

		// S R T -> S �� ������ ���� -> ī�޶��� World ��ȯ ���
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	std::pair<float, float> EditorCamera::panSpeed() const
	{
		// Cherno �� ��𼱰� ������ �Լ�
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::rotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void EditorCamera::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(Key::LeftAlt))
		{
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };

			/*
			���콺 
			�� -> �� : delta.x �� 0 ���� ũ�� 
			�� -> �� : delta.x �� 0 ���� �۴�

			�� -> �Ʒ� : delta.y �� �����
			�Ʒ� -> �� : delta.y �� ������
			*/
			glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;

			m_InitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
				mousePan(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
				mouseRotate(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
				mouseZoom(delta.y);
		}

		updateView();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(EditorCamera::onMouseScroll));
	}

	bool EditorCamera::onMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		mouseZoom(delta);
		updateView();
		return false;
	}

	void EditorCamera::mousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = panSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void EditorCamera::mouseRotate(const glm::vec2& delta)
	{
		/*
		GetUpDirection().y  == 0 ?
		== ī�޶� ���� ���� ���� �ִٴ� �ǹ�
		
		GetUpDirection().y < 0 ?
		> ������ �ٶ󺸰� �ִٴ� ��

		GetUpDirection().y < 0 ?
		> ���� �ڷ� ������ ������ �ٶ󺸰� �ִٴ� ��

		(���� ��ũ)
		https://www.notion.so/OpenGL-Hazel-0415bd834e3c481ea358e9eb08ca52e5

		"���콺 ����"

		���⼭ �߿��� ����, ī�޶� �ڱ� ���� ������ ȥ�� 
		���� �¿� �������� ȸ���ϴ� ���� �ƴϴ�.
		��, ȸ�� ���� ������, ī�޶� ��ġ�� �ƴ϶�
		m_Focal Point �̴�.

		���� 1) ���콺 ������ -> ����
		- delta.x < 0 
		- ī�޶� ����������, �������� ȸ���ϴ� ������ �ƴϴ�
		- �տ� �ڽ��� ������, m_Focal Point �������� ������ �������� 
		  ���ư��鼭, �ڽ��� ������ ���� ���̴�.
		- �� ���� ���, ī�޶� ������ �ƴ϶�, ���� �������� ȸ���Ѵٴ� ����
		  �ǹ��Ѵ�.
		
		��. �׷��� ���⼭ ������ �ϳ� �� �ٴ´�.
		1_1) ���� ������ �ٶ󺸰� �־��ų�
		- �� ���¿��� ī�޶� ���� ȸ����, world �� y �� ���� ȸ���̴�.
		  y �� ���� ȸ����, �ݽð���� ȸ���̰�
		  ������ ��ǥ�迡�� �̴� ����� ȸ���� �ǹ��Ѵ�.
		- ���� Yaw �� ������� ���ϴ� ���̴�.

		1_2) ���� ������ ������ ��ǻ� �ٶ󺸰� �־��ų�
		(���� ��ٰ� �ٷ� ������ �� ������ �ٶ󺸴� ��)

		- �ݴ�� �� ���� ��� world y �� ���� ������ ȸ���� ����.
		  �ð� ���� ȸ���̰�
		  �̴� ������ ��ǥ�� ����, Yaw �� �������� ���ϴ� ���̴�.

		���� 2) ���콺 �Ʒ� -> ��
		- delta.y �� �������̴�. ���� ������ ū ������ �� ��.
		���� ������Ʈ�� �����ϸ�, ī�޶� ���� ���� Box �� �Ʒ����� �ٶ󺸱�
		�����Ѵ�.
		�ٷ� �տ��� �Ʒ����� ���ٴ� ����

		��� ī�޶� ���忡����, ���� ������ �ٶ󺻴ٴ� ���̴�.
		Focal Point �� World ��ǥ�� (0,0,0) ���� ����������

		ī�޶�� ��� ���� ������ ���ϰ� �ȴٴ� ���̴�.

		������ ���Ѵٴ� ����, X �� ���� �ݽð� ���� ȸ���� ���̰�
		�̴� ������ ��ǥ�迡�� ����� ȸ���̴�.

		���� Pitch �� ������� ���ϴ� ���̴�.
		*/

		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;

		m_Yaw += yawSign * delta.x * rotationSpeed();

		m_Pitch += delta.y * rotationSpeed();

		/*
		glm::vec3 forwardVec = GetForwardDirection();
		HZ_CORE_INFO("Camera For Vector {0} {1} {2}", forwardVec.x, forwardVec.y, forwardVec.z);
		*/

		/*
		glm::vec3 orientVec = glm::vec3(-m_Pitch, -m_Yaw, 0.0f);
		HZ_CORE_INFO("Camera For Vector {0} {1} {2}", orientVec.x, orientVec.y, orientVec.z);
		*/
	}

	void EditorCamera::mouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();

		if (m_Distance < 1.0f)
		{
			glm::vec3 forwardVec = GetForwardDirection();
			m_FocalPoint += forwardVec;
			m_Distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		//  glm::vec3(0.0f, 1.0f, 0.0f)) : ���� up direction
		// ���� up ���� �������� x,y ������ ��ŭ ȸ���ߴ°��� ���� ���̴�.
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		// x �� ȸ��
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		/*
		1) glm::vec3(0.0f, 0.0f, -1.0f)
		- ���� ������Ʈ �󿡼� ī�޶��� Initiali �������� ���δ�.
		- OpenGL �� ������ ��ǥ���̰�, Z ������ �츮 ���� �����̴�
		- �׷��� ī�޶�� ������ �ٶ󺸱� ������, -1 * Z ������
		  ���� ī�޶� �ٶ󺸴� �����̴�.
		- ������ ȸ�� ������ �������ִ� ������ ���δ�.

		2) glm::rotate
		- quarternion�� ��õ� ȸ������ vec3 ���·� ��ȯ�ؼ�
		  �������ش�.

		*/
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::calculatePosition() const
	{
		/*
		ex) m_FocalPoint �� (0,0,0), Distance �� 10 �̸�
		Editor Camera �� World Pos �� ��ǻ� z �� -10 �� �ǰ�
		�� ���� �ｼ z = 0 ���� ���� �ڿ��� World �� �ִ�
		������ �ٶ󺸰� �ȴٴ� �ǹ̰� �ȴ�.
		*/
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		/*
		1) ���� ī�޶��� Pitch, Yaw ���� �̿��ؼ� 3D Vector �� ����� �� ����.
		- ȸ�������� '-' �ؼ� ������شٴ� ���� �ٽ��̴�
		- �� '-' �� ���ִ°��� �� �𸣰ڴ�. 
		2) glm::quat �Լ��� �̿��ؼ� 3d vector ��
		quarternion ���·� ��ȯ�Ѵ�.
		*/
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

}