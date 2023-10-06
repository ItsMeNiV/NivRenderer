#include "ECSRegistry.h"

void ECSRegistry::RemoveEntity(const uint32_t entityId)
{
    doRemoveEntity(entityId, true);
}

void ECSRegistry::Reset()
{
    m_Entities.clear();
    m_Components.clear();
    m_EntityComponentsMap.clear();
}

void ECSRegistry::RemoveComponent(const uint32_t entityId, const uint32_t componentId)
{
    auto& componentVector = m_EntityComponentsMap[entityId].second;
    componentVector.erase(std::ranges::remove_if(componentVector, 
        [componentId](const Component* c) { return c->GetId() == componentId; }).begin(), componentVector.end());
    m_Components.erase(componentId);
}

std::vector<Component*> ECSRegistry::GetAllComponents(const uint32_t entityId)
{
    if (!m_EntityComponentsMap.contains(entityId))
	{
        SPDLOG_DEBUG("Entity with ID " + std::to_string(entityId) + " not found!");
        return std::vector<Component*>();
	}

	return m_EntityComponentsMap[entityId].second;
}

void ECSRegistry::doRemoveEntity(uint32_t entityId, bool deleteFromParent)
{
    // Remove child entities
    const auto entity = m_Entities[entityId].get();
    for (const auto child : entity->GetChildEntities())
    {
        doRemoveEntity(child->GetId(), false);
    }

    // Remove entity from parent
    if (deleteFromParent && entity->GetParentEntityId() != -1)
        m_Entities[entity->GetParentEntityId()]->RemoveChildEntity(entityId);

    // Remove attached Components
    m_Entities.erase(entityId);
    for (const auto component : m_EntityComponentsMap[entityId].second)
    {
        m_Components.erase(component->GetId());
    }

    // Remove Entity itself
    m_EntityComponentsMap.erase(entityId);
}
