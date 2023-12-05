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
		m_Registry.each([this](entt::entity entity) {
			if (m_Registry.valid(entity))
			{
				m_Registry.destroy(entity);
			}
			else
			{
				assert(false);
			}
		});
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

		serializer->Save("SceneName", m_Name);

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
		serializer->BeginLoadMap(Reflection::GetTypeID<Scene>(), this);

		serializer->Load("SceneName", m_Name);

		serializer->LoadKey("Entities");

		size_t numActiveEntities = serializer->BeginLoadSeq();

		for (size_t i = 0; i < numActiveEntities; ++i)
		{
			Entity entity{ m_Registry.create(), this };

			deserializeEntity(serializer, entity);
		}
		serializer->EndLoadSeq();

		serializer->EndLoadMap();
	}

	Entity Scene::GetEntityByName(std::string_view name)
	{
		entt::entity foundEntity;
		bool found = false;

		m_Registry.view<NameComponent>().each(
			[&found,&foundEntity, name, this]
		(auto entity, const auto& nameComponent) 
		{
				if (nameComponent.name == name)
				{
					foundEntity = entity;
					found = true;
				}
			}
		);

		if (!found)
		{
			assert(false);
		}

		return Entity{ foundEntity, this };
	}

	void Scene::serializeEntity(Serializer* serializer, Entity entity)
	{
		serializer->BeginSaveMap(Reflection::GetTypeID<Entity>(), this);

		std::vector<const Component*> components = entity.GetComponents();

		// type 정보들 저장하기 
		serializer->SaveKey("types");
		serializer->BeginSaveSeq(components.size());

		for (const Component* constComp : components)
		{
			Reflection::TypeInfo* compTypeInfo = Reflection::GetTypeInfo(constComp->GetType());
			serializer->Save(compTypeInfo->m_Type.GetId());
		}

		serializer->EndSaveSeq();

		// Data 들 저장하기 
		serializer->SaveKey("compDatas");
		serializer->BeginSaveSeq(components.size());

		for (const Component* constComp : components)
		{
			Component* comp = const_cast<Component*>(constComp);
			comp->Serialize(serializer);
		}
		serializer->EndSaveSeq();

		serializer->EndSaveMap();
	}
	void Scene::deserializeEntity(Serializer* serializer, Entity entity)
	{
		serializer->BeginLoadMap(Reflection::GetTypeID<Entity>(), this);

		// types
		serializer->LoadKey("types");
		size_t componentCnt = serializer->BeginLoadSeq();

		std::vector<Component*> vecComponents;
		vecComponents.reserve(componentCnt);

		for (size_t i = 0; i < componentCnt; ++i)
		{
			uint64 typeId;
			serializer->Load(typeId);
			Component* newComponent = addComponentOnDeserialize(TypeId(typeId), entity);
			vecComponents.push_back(newComponent);
		}
		serializer->EndLoadSeq();

		// compDatas
		serializer->LoadKey("compDatas");
		componentCnt = serializer->BeginLoadSeq();

		for (size_t i = 0; i < componentCnt; ++i)
		{
			vecComponents[i]->Deserialize(serializer);
		}

		serializer->EndLoadSeq();

		serializer->EndLoadMap();
	}
	Component* Scene::addComponentOnDeserialize(TypeId type, Entity entity)
	{
		if (type == Reflection::GetTypeID<NameComponent>())
		{
			return &entity.AddComponent<NameComponent>();
		}
		else if (type == Reflection::GetTypeID<CameraComponent>())
		{
			return &entity.AddComponent<CameraComponent>();
		}
		else if (type == Reflection::GetTypeID<SpriteRenderComponent>())
		{
			return &entity.AddComponent<SpriteRenderComponent>();
		}
		else if (type == Reflection::GetTypeID<TransformComponent>())
		{
			return &entity.AddComponent<TransformComponent>();
		}
		else if (type == Reflection::GetTypeID<NativeScriptComponent>())
		{
			return &entity.AddComponent<NativeScriptComponent>();
		}

		assert(false);
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