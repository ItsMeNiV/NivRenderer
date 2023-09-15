#pragma once
#include "Base.h"
#include "Component.h"
#include "PropertyType.h"
#include "Application/Serialization/Serializable.h"

class Entity : public Serializable
{
public:
    virtual ~Entity() = default;

    void AddChildEntity(Entity* entity)
	{
		m_ChildEntities.push_back(entity);
		entity->SetParentEntityId(m_EntityId);
	}
	void RemoveChildEntity(uint32_t childId)
	{
		m_ChildEntities.erase(std::ranges::remove_if(m_ChildEntities, [childId](const Entity* e) {
            return e->GetId() == childId;
        }).begin());
	}

	virtual std::vector<std::pair<std::string, Property>> GetEntityProperties() = 0;

    uint32_t GetId() const { return m_EntityId; }
	std::string* GetEntityName() { return &m_EntityName; }
	const std::vector<Entity*>& GetChildEntities() const { return m_ChildEntities; }
    int32_t GetParentEntityId() const { return m_ParentEntityId; }
	void SetParentEntityId(const uint32_t parentId) { m_ParentEntityId = parentId; }
	bool GetDirtyFlag() const { return m_DirtyFlag; }
	void SetDirtyFlag(bool dirtyFlag) { m_DirtyFlag = dirtyFlag; }

protected:
	Entity(uint32_t id, std::string&& entityName)
		: m_EntityName(std::move(entityName)), m_EntityId(id), m_DirtyFlag(true), m_ParentEntityId(-1)
    {}

protected:
    std::string m_EntityName;
    uint32_t m_EntityId;

private:
	bool m_DirtyFlag;

	int32_t m_ParentEntityId;
	std::vector<Entity*> m_ChildEntities;
};