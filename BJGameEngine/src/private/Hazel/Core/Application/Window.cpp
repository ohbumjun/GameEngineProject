#include "hzpch.h"
#include "Hazel/Core/Application/Window.h"

namespace Hazel
{
	void Window::Init()
	{
        onInit();
	}

	void Window::Open()
	{
		onOpen();
	}

	void Window::Close()
	{
		onClose();
	}

	void Window::NextFrame()
	{
		onNextFrame();
	}

	void Window::EndFrame()
	{
		onEndFrame();
	}

	void Window::Present()
	{
		onPresent();
	}

	void Window::PeekEvent()
	{
    }
    void Window::Update(float deltatime)
    {
        onUpdate(deltatime);
    }
    void Window::onInit()
    {
    }
    void Window::onUpdate(float deltatime)
    {
    }
    void Window::onOpen()
    {
    }
    void Window::onEndFrame()
    {
    }
    void Window::onClose()
    {
    }
    void Window::onGUI()
    {
    }
    void Window::onPrepare(float deltaTime)
    {
    }
    void Window::onRender(float deltaTime)
    {
    }
    void Window::onNextFrame()
    {
    }
    void Window::onPresent()
    {
    }
    } // namespace Hazel
