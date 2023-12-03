#include "hzpch.h"
#include "Scene.h"
#include "Component/SpriteRenderComponent.h"
#include "Component/CameraComponent.h"
#include "Component/TransformComponent.h"
#include "Component/NativeScriptComponent.h"
#include "Component/NameComponent.h"
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

	Scene::Scene(std::string_view name) :
		m_Name(name)
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
					cameraTransform = &transform.GetTransform();
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

				Renderer2D::DrawQuad(transform.GetTransform(), sprite.color);
			}

			Renderer2D::EndScene();
		}
		
	}
	void Scene::Serialize(Serializer* serializer)
	{
		serializer->BeginSaveMap(Reflection::GetTypeID<Scene>(), this);

		serializer->SaveKey("Entities");

		const size_t numActiveEntities = m_Registry.alive();

		serializer->BeginSaveSeq(numActiveEntities);

		m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, this};
		
			if (!entity) return;

			serializeEntity(serializer, entity);

		});

		serializer->EndSaveSeq();

		serializer->EndSaveMap();
	}

	void Scene::Deserialize(Serializer* serializer)
	{
	}

	void Scene::serializeEntity(Serializer* serializer, Entity entity)
	{
		serializer->BeginSaveMap(Reflection::GetTypeID<Entity>(), this);

		uint32 entityIDUInt = entity;
		std::string entityIDStr = std::to_string(entityIDUInt);
		serializer->Save("ID", entityIDStr);

		std::vector<const Component*> components = entity.GetComponents();

		// type 정보들 저장하기 
		serializer->SaveKey("types");
		serializer->BeginSaveSeq(components.size());

		for (const Component* constComp : components)
		{
			serializer->BeginSaveMap();
			Component* comp = const_cast<Component*>(constComp);
			Reflection::TypeInfo* compTypeInfo = Reflection::GetTypeInfo(comp->GetType());
			serializer->Save(compTypeInfo->m_Name.c_str(), comp->GetType());
			serializer->EndSaveMap();
		}

		serializer->EndSaveSeq();

		// Data 들 저장하기 
		serializer->SaveKey("compDatas");
		serializer->BeginSaveSeq(components.size());

		for (const Component* constComp : components)
		{
			Component* comp = const_cast<Component*>(constComp);
			Reflection::TypeInfo* compTypeInfo = Reflection::GetTypeInfo(comp->GetType());
			serializer->SaveKey(compTypeInfo->m_Name.c_str());
			comp->Serialize(serializer);
		}
		serializer->EndSaveSeq();

		serializer->EndSaveMap();
	}
	void Scene::deserializeEntity(Serializer* serializer, Entity entity)
	{
		std::vector<const Component*> components = entity.GetComponents();

		for (const Component* constComp : components)
		{
			Component* comp = const_cast<Component*>(constComp);
			comp->Deserialize(serializer);
		}
	}
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity{ m_Registry.create(), this };

		entity.AddComponent<TransformComponent>();
		entity.AddComponent<NameComponent>(name);

		return entity;
	}
	void Scene::DestroyEntity(const Entity& entity)
	{
		m_Registry.destroy(entity);
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
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<SpriteRenderComponent>(Entity entity, SpriteRenderComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
	template<>
	void Scene::OnComponentAdded<NameComponent>(Entity entity, NameComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}
}