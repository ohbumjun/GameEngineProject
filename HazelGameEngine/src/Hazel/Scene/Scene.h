#pragma once

#include "entt.hpp"

namespace Hazel
{
	class Scene
	{
	public :
		Scene();
		virtual ~Scene();

	private :
		/*
		* ecs 내 모든 component data + entity id 정보를 담는 container
		*/
		entt::registry m_Registry;
	};
}

