#pragma once

#include "entt.hpp"
#include "Scene.h"

namespace Hazel
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity hanel, Scene* scene);
		Entity(const Entity& other) = default;

		// universal reference
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			HZ_CORE_ASSERT(HasComponent<T>() == false, "Component Already Exist");

			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		};
		template<typename T>
		T& GetComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>() == true, "Component Does not Exist");

			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.try_get<T>(m_EntityHandle) ? true : false;
		}
		template<typename T>
		void RemoveComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>() == true, "Component Does not Exist");

			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		};

		operator bool() const { return m_EntityHandle != entt::null; }

		operator uint32_t () const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }

		bool operator == (const Entity& other) const 
		{ 
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}
		bool operator != (const Entity& other) const
		{
			return !(*this == other);
		}

	private:
		entt::entity m_EntityHandle{ entt::null };

		class Scene* m_Scene;
	};
};