#pragma once

#include <glm/glm.hpp>

namespace Hazel
 {
    namespace  Math
    {

    // Transform mat4 ���� s, r, t ������ �̾Ƴ��ִ� �Լ�
    bool HAZEL_API  DecomposeTransform(const glm::mat4 &transform,
                                      glm::vec3 &outTranslation,
                                      glm::vec3 &outRotation,
                                      glm::vec3 &outScale);
    } // namespace Math
}
