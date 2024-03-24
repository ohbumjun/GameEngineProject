#include "Hazel/Scene/Component/CameraComponent.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "Hazel/Core/Serialization/Serializer.h"
#include "hzpch.h"

namespace Hazel
{

    CameraComponent::CameraComponent()
    {
        Reflection::RegistType<CameraComponent>();
    }

    CameraComponent::CameraComponent(const CameraComponent &other)
        : camera(other.camera), isFixedAspectRatio(other.isFixedAspectRatio),
          isPrimary(other.isPrimary)
    {
        Reflection::RegistType<CameraComponent>();
    }

    CameraComponent::CameraComponent(const glm::mat4 &projection)
        : camera(projection)
    {
    }

    void CameraComponent::Serialize(Serializer *serializer)
    {
        serializer->BeginSaveMap(Reflection::GetTypeID<CameraComponent>(), this);

        TypeInfo *compTypeInfo = Reflection::GetTypeInfo(GetType());
        serializer->Save("compName", compTypeInfo->m_Name.c_str());

        serializer->SaveKey("Camera");
        camera.Serialize(serializer);

        serializer->Save("isPrimary", isPrimary);

        serializer->Save("isFixedAspectRatio", isFixedAspectRatio);

        serializer->EndSaveMap();
    }

    void CameraComponent::Deserialize(Serializer *serializer)
    {
        serializer->BeginLoadMap(Reflection::GetTypeID<CameraComponent>(), this);

        std::string compName;
        serializer->Load("compName", compName);

        serializer->LoadKey("Camera");

        camera.Deserialize(serializer);

        serializer->Load("isPrimary", isPrimary);

        serializer->Load("isFixedAspectRatio", isFixedAspectRatio);

        serializer->EndLoadMap();
    }

    const TypeId CameraComponent::GetType() const
    {
        return Reflection::GetTypeID<CameraComponent>();
    }
} // namespace Hazel