#pragma once

#include "entt.hpp"
#include "Hazel/Core/TimeStep.h"

namespace Hazel
{
	class Practice
	{
		Practice();

		entt::registry m_Registry;
	};

	class Scene
	{
		friend class Entity;
	public :
		Scene();
		virtual ~Scene();

		Entity CreateEntity(const std::string& name = "Entity");

		void OnUpdate(const Timestep& ts);

	private :
		/*
		* ecs 내 모든 component data + entity id 정보를 담는 container
		* entity : component  들이 어디에 속하는지에 대한 id 정보일 뿐이다.
		*/
		entt::registry m_Registry;

	};
}

