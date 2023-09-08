#include "ECSRegistry.h"

void ECSRegistry::RemoveEntity(const uint32_t entityId)
{
	m_EntityComponentsMap.erase(entityId);
}

void ECSRegistry::RemoveComponent(const uint32_t entityId, const uint32_t componentId)
{
    uint32_t i = 0;
    auto& v = m_EntityComponentsMap[entityId].second;
    for (const auto& c : v)
    {
        if (c->GetId() == componentId)
        {
            v.erase(v.begin() + i);
            return;
        }
        i++;
    }
}

std::vector<Ref<Component>> ECSRegistry::GetAllComponents(const uint32_t entityId)
{
	if (!checkIfEntityExists(entityId))
	{
        SPDLOG_DEBUG("Entity with ID " + std::to_string(entityId) + " not found!");
		return std::vector<Ref<Component>>();
	}

	return m_EntityComponentsMap[entityId].second;
}

bool ECSRegistry::checkIfEntityExists(const uint32_t entityId)
{
	return m_EntityComponentsMap.count(entityId);
}
