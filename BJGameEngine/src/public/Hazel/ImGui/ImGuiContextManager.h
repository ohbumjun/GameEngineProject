#pragma once

#include "Hazel/Core/Layer/Layer.h"
#include "Hazel/Event/ApplicationEvent.h"
#include "Hazel/Event/Event.h"
#include "Hazel/Event/KeyEvent.h"
#include "Hazel/Event/MouseEvent.h"

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

    static void CreateContext();

    static void DestroyContext(class ImGuiContext *context);

 private:
    static void setDarkThemeColor();
};
} // namespace Hazel
