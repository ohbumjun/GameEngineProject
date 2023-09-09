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
	public :
		Scene();
		virtual ~Scene();

		entt::entity CreateEntity();
		void OnUpdate(const Timestep& ts);

		entt::registry& Reg() { return m_Registry; }

	private :
		/*
		* ecs 내 모든 component data + entity id 정보를 담는 container
		* entity : component  들이 어디에 속하는지에 대한 id 정보일 뿐이다.
		*/
		entt::registry m_Registry;

		friend class Entity;
	};
}

