#include "Hazel.h"

class SandBox : public Hazel::Application
{
public :
	SandBox()
	{
		PushLayer(new Hazel::ImGuiLayer());
	}
	~SandBox() {};
};

Hazel::Application* Hazel::CreateApplication()
{
	return new SandBox();
}