#pragma once

#include "Hazel/Core/Object/BaseObject.h"

namespace Hazel
{
// Texture 라는 최상위 Class 를 두는 이유
// - Texture2D, 3D 등 여부가 관계없이 Texture 라는 것 자체를 사용할 때가 있으므로
class HAZEL_API Font : public BaseObject
{
public:
    virtual ~Font() = default;

  private:
    ImFont *m_Font;
};
} // namespace Hazel