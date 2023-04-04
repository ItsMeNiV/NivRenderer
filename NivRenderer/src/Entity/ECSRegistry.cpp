#include "ECSRegistry.h"

void ECSRegistry::AddEntity(Ref<Entity> entity)
{
	m_Entities.push_back(entity);
	m_EntityComponentsMap[entity->GetId()] = std::vector<Ref<Component>>();
}

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

void ECSRegistry::AddComponent(uint32_t entityId, Ref<Component> component)
{
	bool found = false;
	for (Ref<Entity> e : m_Entities)
	{
		if (e->GetId() == entityId)
			found = true;
	}

	if (!found)
		std::cerr << "Entity with ID " << std::to_string(entityId) << " not found!" << std::endl;
	m_EntityComponentsMap[entityId].push_back(component);
}