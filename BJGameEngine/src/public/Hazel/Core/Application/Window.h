#pragma once

#include "hzpch.h"

#include "Hazel/Core/Core.h"
#include "Hazel/Event/Event.h"

// Platform Dependent
namespace Hazel
{
struct WindowProps
{
    std::string Title;
    uint32_t Width;
    uint32_t Height;
    HINSTANCE m_Inst;

    WindowProps(const std::string &title = "Hazel Engine",
                uint32_t width = 1200,
                uint32_t height = 720)
        : Title(title), Width(width), Height(height)
    {
    }
};

// Interface representing a desktop system based window
class HAZEL_API Window
{
public:
    using EventCallbackFn = std::function<void(Event &)>;
    
    static Window *Create(const WindowProps &props = WindowProps());
    
    virtual ~Window()
    {
    }

    void Init();

    void Open();

    void Close();

    // @brief Update 전에 호출되는 함수
    void NextFrame();

    // @brief Frame 을 정리해주는 함수
    void EndFrame();

    // @breif 화면에 그려주는 함수 ?
    void Present();

    // @brief Event 
    virtual void PeekEvent();

    // virtual void OnUpdate() = 0;
    virtual void Update(float deltatime);

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    // Window Attributes
    virtual void SetEventCallback(const EventCallbackFn &callback) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;

    // ex) OpenGL -> return GLFW Window
    virtual void *GetNativeWindow() const = 0;

    protected:
protected:
    virtual void onInit();

    virtual void onUpdate(float deltatime);

    virtual void onOpen();

    virtual void onEndFrame();

    virtual void onClose();

    virtual void onGUI();

    virtual void onPrepare(float deltaTime);

    virtual void onRender(float deltaTime);

    virtual void onNextFrame();

    virtual void onPresent();
};

} // namespace Hazel