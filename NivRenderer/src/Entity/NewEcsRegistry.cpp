#include "IdManager.h"
#include "NewECSRegistry.h"

NewECSRegistry::NewECSRegistry() : m_Registry(CreateScope<entt::registry>()) {}

void NewECSRegistry::RemoveEntity(const uint32_t entityId)
{
    if (!m_Entities.contains(entityId))
        SPDLOG_ERROR("Entity with Id {} does not exist", entityId);

    if (m_Registry->storage<entt::entity>().remove(m_Entities[entityId].entityHandle))
        m_Entities.erase(entityId);
}

void NewECSRegistry::Reset() const
{
    m_Registry->clear<>();
}

NewEntity& NewECSRegistry::CreateEntity(NewScene* scene)
{
    const uint32_t entityId = IdManager::GetInstance().CreateNewId();
    m_Entities[entityId] = {entityId, scene, m_Registry->create()};
    return m_Entities[IdManager::GetInstance().CreateNewId()];
}

NewEntity* NewECSRegistry::GetEntity(uint32_t entityId)
{
    if (!m_Entities.contains(entityId))
    {
        SPDLOG_ERROR("Entity with Id {} does not exist", entityId);
        return nullptr;
    }

    return &m_Entities[entityId];
}