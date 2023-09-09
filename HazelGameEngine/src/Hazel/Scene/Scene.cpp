#include "hzpch.h"
#include "Scene.h"
#include <glm/glm.hpp>

namespace Hazel
{
	static void OnTransformConstruct() {};

	Practice::Practice()
	{
		struct MeshComponent
		{
			float value;
		};
		struct TransformComponent
		{
			glm::mat4 transform;
			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;
			TransformComponent(const glm::mat4& trans) :
				transform(trans){}
		
			operator const glm::mat4& () const { return transform; }
			operator glm::mat4&() { return transform; }
		};

		entt::entity entity = m_Registry.create();
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.f));

		// Transform Component 생성 때마다 해당 함수 호출
		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();
		
		if (m_Registry.try_get<TransformComponent>(entity))
		{
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
		}

		// TransformComponent  를 가진 모든 entity 리턴
		auto view = m_Registry.view<TransformComponent>();

		for (const auto& entity : view)
		{
			TransformComponent& transform1 = m_Registry.get<TransformComponent>(entity);
			TransformComponent& transform2 = view.get<TransformComponent>(entity);
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);

		for (const auto& entity : group)
		{
			auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

		}
	}

	Scene::Scene()
	{
	}
	Scene::~Scene()
	{
	}
	
}