#pragma once
#include "Base.h"
#include "Component.h"
#include "PropertyType.h"
#include "Application/Serialization/Serializable.h"

class Entity : public Serializable
{
public:
    virtual ~Entity() {}

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

	virtual std::vector<std::pair<std::string, Property>> GetEntityProperties() = 0;

	const uint32_t GetId() const { return m_EntityId; }
	std::string* const GetEntityName() { return &m_EntityName; }
	const std::vector<Ref<Entity>>& GetChildEntities() const { return m_ChildEntities; }
	const int32_t GetParentEntityId() const { return m_ParentEntityId; }
	void SetParentEntityId(uint32_t parentId) { m_ParentEntityId = parentId; }
	bool GetDirtyFlag() const { return m_DirtyFlag; }
	void SetDirtyFlag(bool dirtyFlag) { m_DirtyFlag = dirtyFlag; }

protected:
	Entity(uint32_t id, std::string&& entityName)
		: m_EntityId(id), m_EntityName(std::move(entityName)), m_ChildEntities(), m_ParentEntityId(-1), m_DirtyFlag(true)
	{}

protected:
    std::string m_EntityName;
    uint32_t m_EntityId;

private:
	bool m_DirtyFlag;

	int32_t m_ParentEntityId;
	std::vector<Ref<Entity>> m_ChildEntities;
};