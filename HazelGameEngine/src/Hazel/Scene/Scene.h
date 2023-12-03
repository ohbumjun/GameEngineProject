#pragma once

#include "entt.hpp"
#include "Hazel/Utils/TimeStep.h"
#include "Hazel/Core/Serialization/SerializeTarget.h"

class Scene;

namespace Hazel
{
	class Practice
	{
		Practice();

		entt::registry m_Registry;
	};

	class Entity;

	class Scene : public SerializeTarget
	{
		friend class Entity;
		/*Engine code 인데 Editor Class 를 여기에 friend 로 선언해도 되는 건가 ?*/
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
	public :
		Scene(std::string_view name);
		Scene() {};
		virtual ~Scene();

		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(const Entity& entity);
		void OnViewportResize(uint32_t width, uint32_t height);
		void OnUpdate(const Timestep& ts);

		virtual void Serialize(Serializer* serializer) override;
		virtual void Deserialize(Serializer* serializer) override;
		
		// Component 추가시 호출되는 함수
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		const std::string& GetName()
		{
			return m_Name;
		}

		void SetName(std::string_view name)
		{
			m_Name = name;
		}
	private :

		void serializeEntity(Serializer* serializer, Entity entity);
		void deserializeEntity(Serializer* serializer, Entity entity);

		/*
		* ecs 내 모든 component data + entity id 정보를 담는 container
		* entity : component  들이 어디에 속하는지에 대한 id 정보일 뿐이다.
		*/
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		std::string m_Name;
	};
}

