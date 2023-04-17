#include "ECSRegistry.h"

void ECSRegistry::RemoveEntity(uint32_t entityId)
{
	m_EntityComponentsMapNew.erase(entityId);
}

std::vector<Ref<Component>> ECSRegistry::GetAllComponents(uint32_t entityId)
{
	if (!checkIfEntityExists(entityId))
	{
		std::cerr << "Entity with ID " << std::to_string(entityId) << " not found!" << std::endl;
		return std::vector<Ref<Component>>();
	}

	return m_EntityComponentsMapNew[entityId].second;
}

uint32_t ECSRegistry::CreateNewEntityId()
{
	return m_NextEntityId++;
}

bool ECSRegistry::checkIfEntityExists(uint32_t entityId)
{
	return m_EntityComponentsMapNew.count(entityId);
}
