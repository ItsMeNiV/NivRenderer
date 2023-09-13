#pragma once

template <typename T>
Ref<T> ECSRegistry::AddComponent(const uint32_t entityId)
{
    if (!m_EntityComponentsMap.contains(entityId))
    {
        SPDLOG_DEBUG("Entity with ID " + std::to_string(entityId) + " not found!");
        return nullptr;
    }

    Ref<T> component = CreateRef<T>(IdManager::GetInstance().CreateNewId());

    m_EntityComponentsMap[entityId].second.push_back(component);
    return component;
}

template <typename T>
Ref<T> ECSRegistry::CreateEntity(const int32_t entityIdParent)
{
    static_assert(std::is_base_of_v<Entity, T>);

    Ref<T> entity = CreateRef<T>(IdManager::GetInstance().CreateNewId());
    m_EntityComponentsMap[entity->GetId()].first = entity;
    m_EntityComponentsMap[entity->GetId()].second = std::vector<Ref<Component>>();

    if (entityIdParent != -1)
    {
        m_EntityComponentsMap[entityIdParent].first->AddChildEntity(entity);
    }

    return entity;
}

template <typename T>
Ref<T> ECSRegistry::GetEntity(uint32_t entityId)
{
    auto entity = m_EntityComponentsMap[entityId].first;
    Ref<T> returnPtr = std::dynamic_pointer_cast<T>(entity);
    if (returnPtr)
        return returnPtr;

    SPDLOG_DEBUG("Entity with ID " + std::to_string(entityId) + " not found!");
    return nullptr;
}

template <typename T>
Ref<T> ECSRegistry::GetComponent(const uint32_t entityId)
{
    if (!m_EntityComponentsMap.contains(entityId))
    {
        SPDLOG_DEBUG("Entity with ID " + std::to_string(entityId) + " not found!");
        return nullptr;
    }

    for (const Ref<Component> c : m_EntityComponentsMap[entityId].second)
    {
        Ref<T> returnPtr = std::dynamic_pointer_cast<T>(c);
        if (returnPtr)
            return returnPtr;
    }

    return nullptr;
}