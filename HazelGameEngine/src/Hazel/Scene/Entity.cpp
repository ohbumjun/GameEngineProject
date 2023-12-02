#include "hzpch.h"
#include "Entity.h"

namespace Hazel
{
	Entity::Entity(entt::entity handle, Scene* scene) :
		m_EntityHandle(handle), m_Scene(scene)
	{

	}
	std::vector<Component*> Entity::GetComponents()
	{
		std::vector<Component*> result;

		auto& components = m_Scene->m_Registry.get(m_EntityHandle);

		std::apply([&result](const auto &... args) {

			((result.push_back(&args)), ...);

			}, components);

		return result;
	}
}