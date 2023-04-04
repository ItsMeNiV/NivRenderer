#pragma once
#include "Base.h"

static uint32_t NextEntityId = 0;

class Entity
{
public:
	Entity()
		: m_EntityId(NextEntityId++)
	{}
	virtual ~Entity() {}

	const uint32_t GetId() const { return m_EntityId; }

private:
	uint32_t m_EntityId;

	std::vector<Entity> m_ChildEntities;
};