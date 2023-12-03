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
	-그러면 각 PlatForm 마다 다른 로직은, .h 파일에 구현하는 것이 아니라
	  cpp 에 구현하면
	  1) Abstraction 을 구현 ? 할 수 있게 되고
	  2) Platform 별로 해당 virtual 함수에 링크될, 연결된 cpp 코드들이 
	      컴파일 되게 하는 방향으로 구현할 수 있게 되는 것이다.

		  왜냐하면 예를 들어, Windows Input, Linux Input 관련 코드가
		  하나의 cpp 파일 안에 포함되게 하여
		  같은 Binary 파일로 만들어지는 것을 절대 원하지 않을 것이기 때문이다.

	즉, Input Class 의 함수는 모두 static 으로 declaration 하고
	Window Platform 의 경우, WindowsInput.cpp 에 정의된
	Definition 들이 여기 선언한 Declaration 과 Link 되어
	컴파일 되는 방식을 취할 것이다.
	*/
	class HAZEL_API Input
	{
	public :
		static bool IsKeyPressed(int keyCode) ;
		static bool IsMouseButtonPressed(int button);
		static float GetMouseX();
		static float GetMouseY();
		static std::pair<float, float> GetMousePosition();

	protected :
		// 각 플랫폼 별로 구현할 함수 ex) linux, openGL
		// virtual bool IsKeyPressedImpl(int keyCode) = 0;
		// virtual bool IsMouseButtonPressedImpl(int button) = 0;
		// virtual float GetMouseXImpl() = 0;
		// virtual float GetMouseYImpl() = 0;
		// virtual std::pair<float, float> GetMousePositionImpl() = 0;

	};
}