#pragma once

#include "entt.hpp"
#include "Hazel/Core/ID/UUID.h"
#include "Hazel/Utils/TimeStep.h"
#include "Hazel/Core/Reflection/TypeId.h"
#include "Hazel/Core/Serialization/SerializeTarget.h"
#include "Renderer/Camera/EditorCamera.h"
#include "entt.hpp"

class b2World;

namespace Hazel
{
	class Scene;
	class Component;

	class Practice
	{
		Practice();

		entt::registry m_Registry;
	};

	class Scene : public SerializeTarget
	{
		friend class Entity;
		/*Engine code 인데 Editor Class 를 여기에 friend 로 선언해도 되는 건가 ?*/
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	public :
		
		static Ref<Scene> Copy(Ref<Scene> other);

		Scene(std::string_view name);
		Scene() {};
		virtual ~Scene();

		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		Entity CreateEntity(const std::string& name = "Entity");
		void DuplicateEntity(Entity entity);
		void DestroyEntity(const Entity& entity);
		void OnViewportResize(uint32_t width, uint32_t height);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		/*
		Play 시작 때 호출되는 Scene 쪽의 함수
		*/
		void OnRuntimeStart();

		/*
		Play 끝날 때 호출되는 Scene 함수
		*/
		void OnRuntimeStop();
		/*
		1) runtime update function
		2) update function called from editor side

		update 로직은 engine 과 editor 가 완전히 다르게
		수행되어야 한다.
		
		editor 에서는 추가적으로 imgui 등을 그리고 싶을 수
		있기 때문이다.
		*/
		void OnUpdateRuntime(const Timestep& ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);

		/*
		실제  Game Play  없이 Physics 부분만 진행시키는 함수이다.
		*/
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
		virtual void Serialize(Serializer* serializer) override;
		virtual void Deserialize(Serializer* serializer) override;

		void OnSimulationStart();
		void OnSimulationStop();

		Entity GetPrimaryCameraEntity();
		// Component 추가시 호출되는 함수
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		Entity GetEntityByName(std::string_view name);

		const std::string& GetName()
		{
			return m_Name;
		}

		void SetName(std::string_view name)
		{
			m_Name = name;
		}
	private :

		void onPhysics2DStart();
		void onPhysics2DStop();

		/*
		Editor Camera 로 Scene 을 draw 하기 때문에
		editor 에서 scene 을 render 할 때 사용되는 함수이다.
		*/
		void renderScene(EditorCamera& camera);

		void serializeEntity(Serializer* serializer, Entity entity);
		void deserializeEntity(Serializer* serializer, Entity entity);

		Component* addComponentOnDeserialize(TypeId type, Entity entity);
		/*
		* ecs 내 모든 component data + entity id 정보를 담는 container
		* entity : component  들이 어디에 속하는지에 대한 id 정보일 뿐이다.
		*/
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;
		std::string m_Name;

		/*
		Physic World 를 가지고 있는 전체 World 개념

		- Trasform 을 Controll 하는 것은 이 Physics World 가 
		된다.
		*/
		b2World* m_PhysicsWorld = nullptr;

		
	};
}

