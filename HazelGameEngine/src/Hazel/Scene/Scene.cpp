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
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

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
		{
			// Native Sript Update
			m_Registry.view<NativeScriptComponent>().each([=](auto& nativeComp)
			{
				// TODO : move to scene play ex) onPlayScene
				if (nativeComp.m_Instance == nullptr)
				{
					nativeComp.m_Instance = nativeComp.OnInstantiateScript();

					// 여기서 entity 를 세팅할 수 없다. each 함수가 인자를 하나만 받는 듯.
					nativeComp.m_Instance->OnCreate();
				};
			
				nativeComp.m_Instance->OnUpdate(ts);
			});
		}

		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;

		{
			auto view = m_Registry.view<CameraComponent, TransformComponent>();

			for (auto& entity : view)
			{
				auto [camera, transform] = view.get<CameraComponent, TransformComponent>(entity);
			
				// Primary Camera 로 현재 Scene 을 바라본다.
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
				// auto& 가 필요없다. 왜냐하면 group.get 의 리턴값은  tuple<comp&, comp&> 이다.
				// 즉, tuple 자체를 굳이 & 로 받을 필요도 없을 뿐더러, 이미 compont 정보들은 & 로 리턴한다.
				auto [transform, sprite] = group.get<TransformComponent, SpriteRenderComponent>(entity);

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

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// height : orthographic size
		// width  : orthographic size (viewport size) * aspect ratio
		auto cameras = m_Registry.view<CameraComponent>();

		for (auto& entity : cameras)
		{
			auto& cameraComp = cameras.get<CameraComponent>(entity);

			if (cameraComp.isFixedAspectRatio == false)
			{
				cameraComp.camera.SetViewportSize(width, height);
			}
		}
	}
}