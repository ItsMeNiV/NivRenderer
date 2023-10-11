#include "IdManager.h"
#include "ECSRegistry.h"

ECSRegistry::ECSRegistry() : m_Registry(CreateScope<entt::registry>()) {}

void ECSRegistry::RemoveEntity(const uint32_t entityId)
{
    if (!m_Entities.contains(entityId))
        SPDLOG_ERROR("Entity with Id {} does not exist", entityId);

    m_Registry->destroy(m_Entities[entityId].entityHandle);
    m_Entities.erase(entityId);
}

void ECSRegistry::Reset() const
{
    m_Registry->clear<>();
}

Entity& ECSRegistry::CreateEntity(Scene* scene)
{
    const uint32_t entityId = IdManager::GetInstance().CreateNewId();
    m_Entities[entityId] = {entityId, scene, m_Registry->create()};
    return m_Entities[entityId];
}

Entity* ECSRegistry::GetEntity(uint32_t entityId)
{
    if (!m_Entities.contains(entityId))
    {
        SPDLOG_ERROR("Entity with Id {} does not exist", entityId);
        return nullptr;
    }

    return &m_Entities[entityId];
}