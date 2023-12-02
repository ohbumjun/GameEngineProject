#include "hzpch.h"
#include "Camera.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "Hazel/Core/Serialization/Serializer.h"

namespace Hazel
{
	void Camera::Serialize(Serializer* serializer)
	{
		serializer->BeginSaveMap(Reflection::GetTypeID<Camera>(), this);

		serializer->Save("projectionMatrix", m_ProjectionMatrix);

		serializer->EndSaveMap();
	}
	void Camera::Deserialize(Serializer* serializer)
	{
		serializer->BeginLoadMap(Reflection::GetTypeID<Camera>(), this);

		serializer->Load("projectionMatrix", m_ProjectionMatrix);

		serializer->EndLoadMap();
	}
}