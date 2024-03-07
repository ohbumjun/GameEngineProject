#pragma once

#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{
class HAZEL_API TransformComponent : public Component
{
    friend class Scene;
    // glm::mat4 transform = glm::mat4(1.f);


public:
    TransformComponent();
    TransformComponent(const TransformComponent &other);
    TransformComponent(const glm::vec3 &translation) : Translation(translation)
    {
        TransformComponent();
    }

    virtual void Serialize(Serializer *serializer);
    virtual void Deserialize(Serializer *serializer);

    glm::mat4 GetTransform() const;

    const glm::vec3 &GetTranslation()
    {
        return Translation;
    }
    const glm::vec3 &GetRotation()
    {
        return Rotation;
    }
    const glm::vec3 &GetScale()
    {
        return Scale;
    }

    glm::vec3 &GetTranslationRef()
    {
        return Translation;
    }
    glm::vec3 &GetRotationRef()
    {
        return Rotation;
    }
    glm::vec3 &GetScaleRef()
    {
        return Scale;
    }

    void SetRotation(const glm::vec3 &rot)
    {
        Rotation = rot;
    }
    void SetTranslation(const glm::vec3 &trans)
    {
        Translation = trans;
    }
    void SetScale(const glm::vec3 &scale)
    {
        Scale = scale;
    }

    virtual const TypeId GetType() const;

private:
    glm::vec3 Translation = {0.f, 0.f, 0.f};
    glm::vec3 Rotation = {0.f, 0.f, 0.f};
    glm::vec3 Scale = {1.f, 1.f, 1.f};
};
} // namespace Hazel
