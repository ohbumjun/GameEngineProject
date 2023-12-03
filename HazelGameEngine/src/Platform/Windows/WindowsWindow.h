#pragma once

#include "Hazel/Core/Window.h"

#include <GLFW/glfw3.h>

#include "Renderer/GraphicsContext.h"

namespace Hazel
{
	class WindowsWindow : public Window
	{
	public :
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window Attribute
		virtual void SetEventCallback(const EventCallbackFn& callback) override
		{
			m_Data.EventCallback = callback;
		}
		void SetVSync(bool enabled);
		bool IsVSync() const;

		inline virtual void* GetNativeWindow() const { return m_Window; }
	private :
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private :
		// Handle To Window
		GLFWwindow* m_Window;

		GraphicsContext* m_Context;

		// 아래 struct 는 GLFW 에 넘겨주기 위한 데이터이다.
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			// Application.h 의 OnEvent 가 여기에 세팅될 것이다.
			EventCallbackFn EventCallback;
		};

		WindowData m_Data;

		HINSTANCE m_hInstance;
	};

}


