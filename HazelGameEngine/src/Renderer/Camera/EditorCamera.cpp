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

		// S R T -> S 는 없으니 제외 -> 카메라의 World 변환 행렬
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	std::pair<float, float> EditorCamera::panSpeed() const
	{
		// Cherno 가 어디선가 복사한 함수
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
			마우스 
			왼 -> 오 : delta.x 가 0 보다 크다 
			오 -> 왼 : delta.x 가 0 보다 작다

			위 -> 아래 : delta.y 가 양수값
			아래 -> 위 : delta.y 가 음수값
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
		== 카메라가 수직 위를 보고 있다는 의미
		
		GetUpDirection().y < 0 ?
		> 앞쪽을 바라보고 있다는 것

		GetUpDirection().y < 0 ?
		> 몸을 뒤로 제껴서 뒤쪽을 바라보고 있다는 것

		(참고 링크)
		https://www.notion.so/OpenGL-Hazel-0415bd834e3c481ea358e9eb08ca52e5

		"마우스 기준"

		여기서 중요한 것은, 카메라가 자기 축을 가지고 혼자 
		상하 좌우 기준으로 회전하는 것이 아니다.
		즉, 회전 축의 원점이, 카메라 위치가 아니라
		m_Focal Point 이다.

		예시 1) 마우스 오른쪽 -> 왼쪽
		- delta.x < 0 
		- 카메라가 오른쪽으로, 우측으로 회전하는 개념이 아니다
		- 앞에 박스가 있으면, m_Focal Point 기준으로 오른쪽 방향으로 
		  돌아가면서, 박스의 우측을 보는 것이다.
		- 그 말은 사실, 카메라가 정면이 아니라, 점차 왼쪽으로 회전한다는 것을
		  의미한다.
		
		자. 그런데 여기서 조건이 하나 더 붙는다.
		1_1) 지금 앞쪽을 바라보고 있었거나
		- 이 상태에서 카메라 왼쪽 회전은, world 의 y 축 왼쪽 회전이다.
		  y 축 왼쪽 회전은, 반시계방향 회전이고
		  오른손 좌표계에서 이는 양수값 회전을 의미한다.
		- 따라서 Yaw 에 양수값을 더하는 것이다.

		1_2) 몸을 제껴서 뒤쪽을 사실상 바라보고 있었거나
		(고개를 들다가 바로 위보다 더 뒤쪽을 바라보는 것)

		- 반대로 이 경우는 사실 world y 축 기준 오른쪽 회전과 같다.
		  시계 방향 회전이고
		  이는 오른손 좌표계 에서, Yaw 에 음수값을 더하는 것이다.

		예시 2) 마우스 아래 -> 위
		- delta.y 가 음수값이다. 작은 값에서 큰 값으로 간 것.
		실제 프로젝트를 실행하면, 카메라가 점차 정면 Box 의 아랫면을 바라보기
		시작한다.
		바로 앞에서 아랫면을 본다는 것은

		사실 카메라 입장에서는, 점점 위쪽을 바라본다는 것이다.
		Focal Point 는 World 좌표계 (0,0,0) 으로 고정되지만

		카메라는 사실 점차 위쪽을 향하게 된다는 것이다.

		위쪽을 향한다는 것은, X 축 기준 반시계 방향 회전인 것이고
		이는 오른손 좌표계에서 양수값 회전이다.

		따라서 Pitch 에 양수값을 더하는 것이다.
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
		//  glm::vec3(0.0f, 1.0f, 0.0f)) : 최초 up direction
		// 최초 up 축을 기준으로 x,y 축으로 얼만큼 회전했는가에 대한 값이다.
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		// x 축 회전
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		/*
		1) glm::vec3(0.0f, 0.0f, -1.0f)
		- 현재 프로젝트 상에서 카메라의 Initiali 방향으로 보인다.
		- OpenGL 은 오른손 좌표계이고, Z 방향이 우리 안쪽 방향이다
		- 그런데 카메라는 정면을 바라보기 때문에, -1 * Z 방향이
		  최초 카메라가 바라보는 방향이다.
		- 일종의 회전 기준을 마련해주는 것으로 보인다.

		2) glm::rotate
		- quarternion에 명시된 회전값을 vec3 형태로 변환해서
		  리턴해준다.

		*/
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::calculatePosition() const
	{
		/*
		ex) m_FocalPoint 가 (0,0,0), Distance 가 10 이면
		Editor Camera 의 World Pos 는 사실상 z 축 -10 이 되고
		이 말은 즉슨 z = 0 보다 조금 뒤에서 World 에 있는
		대상들을 바라보게 된다는 의미가 된다.
		*/
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		/*
		1) 먼저 카메라의 Pitch, Yaw 값을 이용해서 3D Vector 를 만드는 것 같다.
		- 회전값들을 '-' 해서 만들어준다는 것이 핵심이다
		- 왜 '-' 를 해주는가는 잘 모르겠다. 
		2) glm::quat 함수를 이용해서 3d vector 를
		quarternion 형태로 변환한다.
		*/
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

}