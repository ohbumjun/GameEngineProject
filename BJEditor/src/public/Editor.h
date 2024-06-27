#pragma once

#include "Panel/ProjectSelectPanel.h"
#include <Hazel.h>

namespace HazelEditor
{
class Editor
{
public:
    static void CreateEditor();

    Editor();
    virtual ~Editor();

    void Initialize();
};
} // namespace HazelEditor
