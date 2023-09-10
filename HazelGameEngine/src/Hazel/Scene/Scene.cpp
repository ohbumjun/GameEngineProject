#include "hzpch.h"
#include "Scene.h"
#include "Component.h"
#include "Renderer/Renderer2D.h"
#include <glm/glm.hpp>
#include "Entity.h"

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
	void Scene::OnUpdate(const Timestep& ts)
	{
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;

		{
			auto group = m_Registry.view<CameraComponent, TransformComponent>();

			for (auto& entity : group)
			{
				auto& [camera, transform] = group.get<CameraComponent, TransformComponent>(entity);
			
				if (camera.isPrimary)
				{
					mainCamera = &camera.camera;
					cameraTransform = &transform.transform;
					break;
				}
			}
		}

		// Render Sprites
		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, *cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRenderComponent>);

			for (const auto& entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRenderComponent>(entity);

				Renderer2D::DrawQuad(transform, sprite.color);
			}

			Renderer2D::EndScene();
		}
		
	};

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity{ m_Registry.create(), this };

		entity.AddComponent<TransformComponent>();
		entity.AddComponent<NameComponent>(name);

		return entity;
	}
}