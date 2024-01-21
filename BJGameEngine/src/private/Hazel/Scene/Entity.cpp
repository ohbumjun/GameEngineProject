#include "hzpch.h"

#include "Hazel/Scene/Entity.h"

namespace Hazel
{
	Entity::Entity(entt::entity handle, Scene* scene) :
		m_EntityHandle(handle), m_Scene(scene)
	{

	}
	std::vector<const Component*> Entity::GetComponents()
	{
		std::vector<const Component*> result;

		auto& components = m_Scene->m_Registry.get(m_EntityHandle);

		// https://github.com/skypjack/entt/issues/88

		int num = 0;

		for (const auto&& compStoragePair : m_Scene->m_Registry.storage())
		{
			entt::id_type id = compStoragePair.first;

			num += 1;

			const auto& componentStorage = compStoragePair.second;

			if (componentStorage.contains(m_EntityHandle) == false)
			{
				continue;
			}

			const Component* compData = reinterpret_cast<const Component*>(
				componentStorage.value(m_EntityHandle));

			result.push_back(compData);
		}

		return result;
	}
}