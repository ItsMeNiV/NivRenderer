#pragma once
#include "Base.h"
#include "..\Application\Scene.h"
#include "Entity.h"
#include "Entity/Components.h"
#include "entt/entt.hpp"

class ECSRegistry
{
public:
    ECSRegistry(class ECSRegistry const&) = delete;
    void operator=(ECSRegistry const&) = delete;

    static ECSRegistry& GetInstance()
    {
        static ECSRegistry instance;

        return instance;
    }


    Entity& CreateEntity(Scene* scene);
    Entity* GetEntity(uint32_t entityId);
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
    std::unordered_map<uint32_t, Entity> m_Entities;

    // Private Constructor
    ECSRegistry();
};

// Template implementations
template <typename T, typename... Args>
T* ECSRegistry::AddComponent(const uint32_t entityId, Args&&... args)
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
T* ECSRegistry::GetComponent(const uint32_t entityId)
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
void ECSRegistry::RemoveComponent(const uint32_t entityId)
{
    if (!m_Entities.contains(entityId))
    {
        SPDLOG_ERROR("Entity with Id {} does not exist", entityId);
        return;
    }

    m_Registry->remove<T>(m_Entities[entityId].entityHandle);
}
