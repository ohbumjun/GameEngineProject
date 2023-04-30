#include <Hazel.h>

// 참고 : imgui 가 include 되는 원리 (혹은 link 원리)
// imgui 를 빌드하여 static library로 만든다.
// (imgui -> static lib)
// 그리고 hazel 로 static lib 형태로 include 가 되게 한다.
// hazel 은 dll 형태로 뽑아지게 된다.
// (hazel -> dll)
// client 는 dll 형태의 hazel 을 include 하는 것이다.
#include "imgui/imgui.h"

class ExampleLayer : public Hazel::Layer
{
public: 
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB))
			HZ_TRACE("Tab key is pressed (poll)!");
	}

	void OnEvent(Hazel::Event& event) override
	{
		if (event.GetEventType() == Hazel::EventType::KeyPressed)
		{
			Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;
			if (e.GetKeyCode() == HZ_KEY_TAB)
				HZ_TRACE("Tab key is pressed (event)!");
			HZ_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		
		// Client 측에서 ImGuiLayer 를 세팅하게 해주고 싶지 않다.
		// Engine 측 Application 에서 추가하게 할 것이다.
		// PushOverlay(new Hazel::ImGuiLayer());
	}

	~Sandbox()
	{

	}

};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}