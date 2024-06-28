#pragma once

#include "Hazel/Core/Layer/Layer.h"
#include "Hazel/Event/ApplicationEvent.h"
#include "Hazel/Event/Event.h"
#include "Hazel/Event/KeyEvent.h"
#include "Hazel/Event/MouseEvent.h"

class ImGuiContext;

namespace Hazel
{
// export 해줘야 한다.
class HAZEL_API ImguiContextManager 
{
public:
    ImguiContextManager();
    ~ImguiContextManager();

    static void Initialize();

    static void Finalize();

    static ImGuiContext *CreateContext();

    static void DestroyContext(ImGuiContext *context);

 private:
    static void setDarkThemeColor();

    static class FreeListAllocator * s_imguiAllocator;
};
} // namespace Hazel
