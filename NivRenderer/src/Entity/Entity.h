#pragma once
#include "Base.h"

static uint32_t NextEntityId = 0;

class Entity
{
public:
	Entity(std::string&& entityName)
		: m_EntityId(NextEntityId++), m_EntityName(std::move(entityName))
	{}
	virtual ~Entity() {}

	void AddChildEntity(Ref<Entity> entity) { m_ChildEntities.push_back(entity); }

	const uint32_t GetId() const { return m_EntityId; }
	std::string* const GetEntityName() { return &m_EntityName; }
	const std::vector<Ref<Entity>>& GetChildEntities() const { return m_ChildEntities; }

private:
	uint32_t m_EntityId;
	std::string m_EntityName;

	std::vector<Ref<Entity>> m_ChildEntities;
};