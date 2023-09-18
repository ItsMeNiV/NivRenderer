#pragma once
#include "Base.h"
#include "Entity/Entity.h"
#include "Entity/Component.h"
#include "IdManager.h"

class ECSRegistry
{
public:
	ECSRegistry(ECSRegistry const&) = delete;
	void operator=(ECSRegistry const&) = delete;

	static ECSRegistry& GetInstance()
	{
		static ECSRegistry instance;

		return instance;
	}

	void RemoveEntity(const uint32_t entityId);
    void ClearRegistry();
    void RemoveComponent(const uint32_t entityId, const uint32_t componentId);
    std::vector<Component*> GetAllComponents(const uint32_t entityId);

	template<typename T>
    T* AddComponent(const uint32_t entityId);

	template<typename T>
    T* CreateEntity(const int32_t entityIdParent = -1);

	template<typename T>
    T* GetEntity(uint32_t entityId);

	template<typename T>
    T* GetComponent(const uint32_t entityId);

private:
    ECSRegistry() = default;

    void doRemoveEntity(uint32_t entityId, bool deleteFromParent);

	std::unordered_map<uint32_t, std::pair<Entity*, std::vector<Component*>>> m_EntityComponentsMap;
    std::unordered_map<uint32_t, Scope<Entity>> m_Entities;
    std::unordered_map<uint32_t, Scope<Component>> m_Components;
};

// Template implementations
template <typename T>
T* ECSRegistry::AddComponent(const uint32_t entityId)
{
    static_assert(std::is_base_of_v<Component, T>);

    if (!m_EntityComponentsMap.contains(entityId))
    {
        SPDLOG_DEBUG("Entity with ID " + std::to_string(entityId) + " not found!");
        return nullptr;
    }

    const uint32_t id = IdManager::GetInstance().CreateNewId();
    m_Components[id] = CreateScope<T>(id);
    Component* component = m_Components[id].get();

    m_EntityComponentsMap[entityId].second.push_back(component);

    return static_cast<T*>(component);
}

template <typename T>
T* ECSRegistry::CreateEntity(const int32_t entityIdParent)
{
    static_assert(std::is_base_of_v<Entity, T>);

    const uint32_t id = IdManager::GetInstance().CreateNewId();
    m_Entities[id] = CreateScope<T>(id);
    Entity* entity = m_Entities[id].get();
    m_EntityComponentsMap[id].first = entity;
    m_EntityComponentsMap[id].second = std::vector<Component*>();

    if (entityIdParent != -1)
    {
        m_Entities[entityIdParent]->AddChildEntity(entity);
    }

    return static_cast<T*>(entity);
}

template <typename T>
T* ECSRegistry::GetEntity(uint32_t entityId)
{
    Entity* entity = m_EntityComponentsMap[entityId].first;
    if (T* returnPtr = dynamic_cast<T*>(entity))
        return returnPtr;

    SPDLOG_DEBUG("Entity with ID " + std::to_string(entityId) + " not found!");
    return nullptr;
}

template <typename T>
T* ECSRegistry::GetComponent(const uint32_t entityId)
{
    if (!m_Entities.contains(entityId))
    {
        SPDLOG_DEBUG("Entity with ID " + std::to_string(entityId) + " not found!");
        return nullptr;
    }

    for (Component* c : m_EntityComponentsMap[entityId].second)
    {
        if (T* returnPtr = dynamic_cast<T*>(c))
            return returnPtr;
    }

    return nullptr;
}
