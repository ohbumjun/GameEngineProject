#include "Hazel/Scene/Component/TransformComponent.h"
#include "Hazel/Core/Reflection/Reflection.h"
#include "Hazel/Core/Serialization/Serializer.h"
#include "hzpch.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

Hazel::TransformComponent::TransformComponent()
{
    Reflection::RegistType<TransformComponent>();
}

Hazel::TransformComponent::TransformComponent(const TransformComponent &other)
    : Translation(other.Translation), Rotation(other.Rotation),
      Scale(other.Scale)
{
    Reflection::RegistType<TransformComponent>();
}

void Hazel::TransformComponent::Serialize(Serializer *serializer)
{
    serializer->BeginSaveMap(Reflection::GetTypeID<TransformComponent>(), this);

    Reflection::TypeInfo *compTypeInfo = Reflection::GetTypeInfo(GetType());
    serializer->Save("compName", compTypeInfo->m_Name.c_str());

    serializer->Save("Translation", Translation);
    serializer->Save("Rotation", Rotation);
    serializer->Save("Scale", Scale);

    serializer->EndSaveMap();
}

void Hazel::TransformComponent::Deserialize(Serializer *serializer)
{
    serializer->BeginLoadMap(Reflection::GetTypeID<TransformComponent>(), this);

    std::string compName;
    serializer->Load("compName", compName);

    serializer->Load("Translation", Translation);
    serializer->Load("Rotation", Rotation);
    serializer->Load("Scale", Scale);

    serializer->EndLoadMap();
}

glm::mat4 Hazel::TransformComponent::GetTransform() const
{
    // x,y,z 회전 적용한 quartenion 형태의 값 가져오기
    // glm::mat4 rotation = glm::rotate(glm::mat4(1.f), Rotation.x, { 1, 0, 0 })
    // 	* glm::rotate(glm::mat4(1.f), Rotation.y, { 0, 1, 0 })
    // 	* glm::rotate(glm::mat4(1.f), Rotation.z, { 0, 0, 1 });

    glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

    /*
	참고 
	>> 위 2가지 방식은 다른 결과값을 가져오게 된다.
	기존에는 첫번째 것을 사용했었다. 하지만 Editor Camera 와 통일성을 맞추기 위해
	quarternion 값 형태로 변환하는 로직을 통일시켰다.

	>> 기존 로직에서는 내가 Editor 상에서 Y 를 + 값으로 바꾸면
	반대로 아래로 내려갔다.
	아마 기존 방식대로 rotation 행렬을 구하게 되면
	부정확한 행렬값이 나오는 것으로 보인다.
	*/

    return glm::translate(glm::mat4(1.0f), Translation) * rotation *
           glm::scale(glm::mat4(1.0f), Scale);
}

const TypeId Hazel::TransformComponent::GetType() const
{
    return Reflection::GetTypeID<TransformComponent>();
}
