#include "Renderer/Camera/OrthographicCameraController.h"
#include "hzpch.h"

#include "Hazel/Input/Input.h"
#include "Hazel/Input/KeyCodes.h"

namespace Hazel
{
// TODO : m_Camera{ -1.6f, 1.6f, -0.9f, 0.9f },
// 왜 -m_AspectRatio* m_ZoomLevel 가 width, height 가 되는 거지 ?
OrthographicCameraController::OrthographicCameraController(float aspectRatio,
                                                           float rotation)
    : m_Rotation(rotation), m_AspectRatio(aspectRatio),
      m_Bounds({-m_AspectRatio * m_ZoomLevel,
                m_AspectRatio * m_ZoomLevel,
                -m_ZoomLevel,
                m_ZoomLevel}),
      m_Camera(-1.f * m_AspectRatio * m_ZoomLevel,
               m_AspectRatio * m_ZoomLevel,
               -m_ZoomLevel,
               m_ZoomLevel)
{
}

void OrthographicCameraController::OnUpdate(Timestep ts)
{
    /*Pos*/
    // if (Input::IsKeyPressed(HZ_KEY_A))
    if (Input::IsKeyPressed(Hazel::Key::A))
    {
        m_CameraPos.x -= m_CameraMoveSpeed * ts;
    }
    // else if (Input::IsKeyPressed(HZ_KEY_D))
    else if (Input::IsKeyPressed(Hazel::Key::D))
    {
        m_CameraPos.x += m_CameraMoveSpeed * ts;
    }

    // if (Input::IsKeyPressed(HZ_KEY_W))
    if (Input::IsKeyPressed(Hazel::Key::W))
    {
        m_CameraPos.y += m_CameraMoveSpeed * ts;
    }
    // else if (Input::IsKeyPressed(HZ_KEY_S))
    else if (Input::IsKeyPressed(Hazel::Key::S))
    {
        m_CameraPos.y -= m_CameraMoveSpeed * ts;
    }

    m_Camera.SetPosition(m_CameraPos);

    // Zoom Level 에 따라 움직임 속도 조절하기
    m_CameraMoveSpeed = m_ZoomLevel;

    /*Rot*/
    if (m_Rotation)
    {
        // if (Input::IsKeyPressed(HZ_KEY_Q))
        if (Input::IsKeyPressed(Hazel::Key::Q))
        {
            m_CameraRot += m_CameraRotSpeed;
        }
        // else if (Input::IsKeyPressed(HZ_KEY_E))
        else if (Input::IsKeyPressed(Hazel::Key::E))
        {
            m_CameraRot -= m_CameraRotSpeed;
        }

        m_Camera.SetRotation(m_CameraRot);
    }
}

void OrthographicCameraController::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(
        HZ_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolledEvent));
    dispatcher.Dispatch<WindowResizeEvent>(
        HZ_BIND_EVENT_FN(OrthographicCameraController::OnWindowResizeEvent));
}

void OrthographicCameraController::OnResize(float width, float height)
{
    m_AspectRatio = width / height;
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel,
                           m_AspectRatio * m_ZoomLevel,
                           -m_ZoomLevel,
                           m_ZoomLevel);
}

bool OrthographicCameraController::OnMouseScrolledEvent(MouseScrolledEvent &e)
{
    // change zoom level by offset
    m_ZoomLevel -= e.GetYOffset();

    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);

    CalculateView();

    return false;
}

bool OrthographicCameraController::OnWindowResizeEvent(WindowResizeEvent &e)
{
    m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();

    CalculateView();

    return false;
}

void OrthographicCameraController::CalculateView()
{
    m_Bounds = {-m_AspectRatio * m_ZoomLevel,
                m_AspectRatio * m_ZoomLevel,
                -m_ZoomLevel,
                m_ZoomLevel};

    // 좌우 크기 조절 => 대상들 크기 변화 X
    // 상하 크기 조절 => 대상들 크기 변화 (비례)

    m_Camera.SetProjection(-1.f * m_AspectRatio * m_ZoomLevel,
                           m_AspectRatio * m_ZoomLevel,
                           -1.f * m_ZoomLevel,
                           m_ZoomLevel);
}

} // namespace Hazel
