#pragma once
#include "Base.h"

static uint32_t NextEntityId = 0;

class Entity
{
public:
	void AddChildEntity(Ref<Entity> entity)
	{
		m_ChildEntities.push_back(entity);
		entity->SetParentEntityId(m_EntityId);
	}
	void RemoveChildEntity(uint32_t childId)
	{
		if (std::find_if(m_ChildEntities.begin(), m_ChildEntities.end(), [childId](Ref<Entity> e) { return e->GetId() == childId; }) == m_ChildEntities.end())
			return;

		m_ChildEntities.erase(std::remove_if(begin(m_ChildEntities), end(m_ChildEntities), [childId](Ref<Entity> e) {
			return e->GetId() == childId;
		}));
	}

	const uint32_t GetId() const { return m_EntityId; }
	std::string* const GetEntityName() { return &m_EntityName; }
	const std::vector<Ref<Entity>>& GetChildEntities() const { return m_ChildEntities; }
	const int32_t GetParentEntityId() const { return m_ParentEntityId; }
	void SetParentEntityId(uint32_t parentId) { m_ParentEntityId = parentId; }

protected:
	Entity(std::string&& entityName)
		: m_EntityId(NextEntityId++), m_EntityName(std::move(entityName)), m_ChildEntities(), m_ParentEntityId(-1)
	{}
	virtual ~Entity() {}

private:
	uint32_t m_EntityId;
	std::string m_EntityName;

	int32_t m_ParentEntityId;
	std::vector<Ref<Entity>> m_ChildEntities;
};