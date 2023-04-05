#include "ECSRegistry.h"

void ECSRegistry::RemoveEntity(uint32_t entityId)
{
	m_Entities.erase(std::remove_if(begin(m_Entities), end(m_Entities), [entityId](Ref<Entity> e) {
		return e->GetId() == entityId;
	}));
	for (auto it = begin(m_EntityComponentsMap); it != end(m_EntityComponentsMap);)
	{
		if (it->first == entityId)
		{
			it = m_EntityComponentsMap.erase(it);
		}
		else
			++it;
	}
}

std::vector<Ref<Component>> ECSRegistry::GetAllComponents(uint32_t entityId)
{
	if (!checkIfEntityExists(entityId))
	{
		std::cerr << "Entity with ID " << std::to_string(entityId) << " not found!" << std::endl;
		return std::vector<Ref<Component>>();
	}

	return m_EntityComponentsMap[entityId];
}

bool ECSRegistry::checkIfEntityExists(uint32_t entityId)
{
	bool found = false;
	for (Ref<Entity> e : m_Entities)
	{
		if (e->GetId() == entityId)
		{
			return true;
		}
	}

	return false;
}
