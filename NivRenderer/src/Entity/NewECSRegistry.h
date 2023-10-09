#pragma once
#include "Base.h"
#include "Application/NewScene.h"
#include "Entity/NewEntity.h"
#include "Entity/Components.h"
#include "entt/entt.hpp"

class NewECSRegistry
{
public:
    NewECSRegistry(class NewECSRegistry const&) = delete;
    void operator=(NewECSRegistry const&) = delete;

    static NewECSRegistry& GetInstance()
    {
        static NewECSRegistry instance;

        return instance;
    }


    NewEntity& CreateEntity(NewScene* scene);
    NewEntity* GetEntity(uint32_t entityId);
    void RemoveEntity(const uint32_t entityId);
    void Reset() const;

    template <typename T, typename... Args>
    T* AddComponent(const uint32_t entityId, Args&&... args);

    template <typename T>
    T* GetComponent(const uint32_t entityId);

    template <typename T>
    void RemoveComponent(const uint32_t entityId);

private:
    Scope<entt::registry> m_Registry;
    std::unordered_map<uint32_t, NewEntity> m_Entities;

    // Private Constructor
    NewECSRegistry();
};

// Template implementations
template <typename T, typename... Args>
T* NewECSRegistry::AddComponent(const uint32_t entityId, Args&&... args)
{
    if (!m_Entities.contains(entityId))
    {
        SPDLOG_ERROR("Entity with Id {} does not exist", entityId);
        return nullptr;
    }

    T& component = m_Registry->emplace<T>(m_Entities[entityId].entityHandle, std::forward<Args>(args)...);

    return &component;
}

template <typename T>
T* NewECSRegistry::GetComponent(const uint32_t entityId)
{
    if (!m_Entities.contains(entityId))
    {
        SPDLOG_ERROR("Entity with Id {} does not exist", entityId);
        return nullptr;
    }

    if (!m_Registry->all_of<T>(m_Entities[entityId].entityHandle))
    {
        SPDLOG_INFO("Entity has no Component of Type {}", typeid(T).name());
        return nullptr;
    }
    
    return m_Registry->try_get<T>(m_Entities[entityId].entityHandle);
}

template <typename T>
void NewECSRegistry::RemoveComponent(const uint32_t entityId)
{
    if (!m_Entities.contains(entityId))
    {
        SPDLOG_ERROR("Entity with Id {} does not exist", entityId);
        return;
    }

    m_Registry->remove<T>(m_Entities[entityId].entityHandle);
}
