#pragma once

#include "Component/Component.h"
#include "Component/Identifier/IDComponent.h"
#include "Component/Identifier/NameComponent.h"
#include "Hazel/Core/ID/UUID.h"
#include "Scene.h"
#include "entt.hpp"

namespace Hazel
{
class HAZEL_API Entity
{
    friend class Scene;

public:
    Entity() = default;
    Entity(entt::entity hanel, Scene *scene);
    Entity(const Entity &other) = default;

    // universal reference
    template <typename T, typename... Args>
    T &AddComponent(Args &&...args)
    {
        HZ_CORE_ASSERT(HasComponent<T>() == false, "Component Already Exist");
        T &newComp =
            m_Scene->m_Registry.emplace<T>(m_EntityHandle,
                                           std::forward<Args>(args)...);
        m_Scene->OnComponentAdded<T>(*this, newComp);
        return newComp;
    };
    template <typename T>
    T &GetComponent()
    {
        HZ_CORE_ASSERT(HasComponent<T>() == true, "Component Does not Exist");
        HZ_CORE_ASSERT(m_Scene, "Scene Does Not Exist");

        return m_Scene->m_Registry.get<T>(m_EntityHandle);
    }

    template <typename T>
    bool HasComponent()
    {
        return m_Scene->m_Registry.try_get<T>(m_EntityHandle) ? true : false;
    }
    template <typename T>
    void RemoveComponent()
    {
        HZ_CORE_ASSERT(HasComponent<T>() == true, "Component Does not Exist");

        m_Scene->m_Registry.remove<T>(m_EntityHandle);
    };
    template <typename T, typename... Args>
    T &AddOrReplaceComponent(Args &&...args)
    {
        T &component = m_Scene->m_Registry.emplace_or_replace<T>(
            m_EntityHandle,
            std::forward<Args>(args)...);
        m_Scene->OnComponentAdded<T>(*this, component);
        return component;
    }
    operator bool() const
    {
        return m_EntityHandle != entt::null;
    }

    operator uint32_t() const
    {
        return (uint32_t)m_EntityHandle;
    }
    operator entt::entity() const
    {
        return m_EntityHandle;
    }

    bool operator==(const Entity &other) const
    {
        return m_EntityHandle == other.m_EntityHandle &&
               m_Scene == other.m_Scene;
    }
    bool operator!=(const Entity &other) const
    {
        return !(*this == other);
    }
    std::vector<const Component *> GetComponents();

    UUID GetUUID()
    {
        return GetComponent<IDComponent>().GetUUID();
    }
    const std::string &GetName()
    {
        return GetComponent<NameComponent>().GetName();
    }

    inline bool IsValid() const
    {
        return m_EntityHandle != entt::null;
    }

private:
    entt::entity m_EntityHandle{entt::null};

    class Scene *m_Scene;
};
}; // namespace Hazel