#include "Renderer/Camera/Camera.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "Hazel/Core/Serialization/Serializer.h"
#include "hzpch.h"

namespace Hazel
{
void Camera::Serialize(Serializer *serializer)
{
    serializer->BeginSaveMap(Reflection::GetTypeID<Camera>(), this);

    serializeData(serializer);

    serializer->EndSaveMap();
}
void Camera::Deserialize(Serializer *serializer)
{
    serializer->BeginLoadMap(Reflection::GetTypeID<Camera>(), this);

    deserializeData(serializer);

    serializer->EndLoadMap();
}
void Camera::serializeData(Serializer *serializer)
{
    serializer->Save("projectionMatrix", m_ProjectionMatrix);
}
void Camera::deserializeData(Serializer *serializer)
{
    serializer->Load("projectionMatrix", m_ProjectionMatrix);
    bool h = true;
}
} // namespace Hazel