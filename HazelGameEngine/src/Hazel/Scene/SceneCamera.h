#pragma once

#include "Renderer/Camera.h"

namespace Hazel
{
	class SceneCamera : public Camera
	{
	public :
		SceneCamera();
		~SceneCamera();
		void SetOrthographic(float size, float nearDis, float farDis);
		/*
		* 해당 함수는 Render 이전에 호출해야 한다.
		*/
		void SetViewportSize(uint32_t width, uint32_t height);
	private :
		float m_OrthographicSize = 10.f;
		float m_OrthographicNear = -1.f, m_OrthographicFar = 1.f;
	};

}



