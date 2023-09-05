#pragma once

#include "Hazel/Core/Core.h"

namespace Hazel
{
	/*
	>> Input Class 구성 방식
	- Windows, Linux, Mac 등 다양한 platform 에 따라 다른 방식의 Input 로직  및
	  코드를 실행해야 한다.
	- 이를 위해 Input 이라는 Base Class 를 두고, 각 PlatForm 에 따라
	  Input Class 를 상속받는 파생 클래스를 둔다.
	- 그리고 single ton 객체를 하나 두고, static 함수 안에서는 해당 객채의
	  virtual 함수를 실행하게 한다.
	-그러면 각 PlatForm 마다 다른 로직은, .h 파일에 구현하는 것이 아니라
	  cpp 에 구현하면
	  1) Abstraction 을 구현 ? 할 수 있게 되고
	  2) Platform 별로 해당 virtual 함수에 링크될, 연결된 cpp 코드들이 
	      컴파일 되게 하는 방향으로 구현할 수 있게 되는 것이다.
	*/
	class HAZEL_API Input
	{
	public :
		inline static bool IsKeyPressed(int keyCode)
		{
			return s_Instance->IsKeyPressedImpl(keyCode);
		};
		inline static bool IsMouseButtonPressed(int button)
		{
			return s_Instance->IsMouseButtonPressedImpl(button);
		};
		inline static float GetMouseX()
		{
			return s_Instance->GetMouseXImpl();
		};
		inline static float GetMouseY()
		{
			return s_Instance->GetMouseYImpl();
		};
		inline static std::pair<float,float> GetMousePosition()
		{
			return s_Instance->GetMousePositionImpl();
		}

	protected :
		// 각 플랫폼 별로 구현할 함수 ex) linux, openGL
		virtual bool IsKeyPressedImpl(int keyCode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;

	private :
		static Input* s_Instance;
	};
}