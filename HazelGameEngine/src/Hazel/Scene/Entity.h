#pragma once

#include "entt.hpp"
#include "Scene.h"

namespace Hazel
{
	class Entity
	{
	public :
		Entity(entt::entity hanel, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.try_get<TransformComponent>(entity) ? true : false;
		}
	private :
		entt::entity m_EntityHandle;

		/*
		Entity 가 Scene 객체에 대한 소유권을 가지게 하고 싶지 않다
		그저 Scene object 에 대한 포인터만 들고 있게 하고 싶다.
		*/
		// Ref<Scene> m_Scene;
		class Scene* m_Scene;
	};
}