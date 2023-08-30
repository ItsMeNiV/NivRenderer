#pragma once
#include "Base.h"
#include "Entity/Entity.h"
#include "Entity/Component.h"
#include "IdManager.h"

class ECSRegistry
{
public:
	ECSRegistry(ECSRegistry const&) = delete;
	void operator=(ECSRegistry const&) = delete;

	static ECSRegistry& GetInstance()
	{
		static ECSRegistry instance;

		return instance;
	}

	void RemoveEntity(uint32_t entityId);
	std::vector<Ref<Component>> GetAllComponents(uint32_t entityId);

	template<typename T>
	void AddComponent(uint32_t entityId)
	{
		if (!checkIfEntityExists(entityId))
		{
            SPDLOG_DEBUG("Entity with ID " + std::to_string(entityId) + " not found!");
			return;
		}

		Ref<T> component = CreateRef<T>(IdManager::GetInstance().CreateNewId());

		m_EntityComponentsMap[entityId].second.push_back(component);
	}

	template<typename T>
	Ref<T> CreateEntity(int32_t entityIdParent = -1)
	{
		static_assert(std::is_base_of_v<Entity, T>);

		Ref<T> entity = CreateRef<T>(IdManager::GetInstance().CreateNewId());
		m_EntityComponentsMap[entity->GetId()].first = entity;
		m_EntityComponentsMap[entity->GetId()].second = std::vector<Ref<Component>>();

		if (entityIdParent != -1)
		{
			m_EntityComponentsMap[entityIdParent].first->AddChildEntity(entity);
		}
			
		return entity;
	}

	template<typename T>
	Ref<T> GetEntity(uint32_t entityId)
	{
		auto entity = m_EntityComponentsMap[entityId].first;
		Ref<T> returnPtr = std::dynamic_pointer_cast<T>(entity);
		if (returnPtr)
			return returnPtr;

		SPDLOG_DEBUG("Entity with ID " + std::to_string(entityId) + " not found!");
		return nullptr;
	}

	template<typename T>
	Ref<T> GetComponent(uint32_t entityId)
	{
		if (!checkIfEntityExists(entityId))
		{
            SPDLOG_DEBUG("Entity with ID " + std::to_string(entityId) + " not found!");
			return nullptr;
		}

		for (const Ref<Component> c : m_EntityComponentsMap[entityId].second)
		{
			Ref<T> returnPtr = std::dynamic_pointer_cast<T>(c);
			if (returnPtr)
				return returnPtr;
		}

		return nullptr;
	}

private:
	ECSRegistry() : m_NextEntityId(0) {}

	uint32_t m_NextEntityId;

	std::unordered_map<uint32_t, std::pair<Ref<Entity>, std::vector<Ref<Component>>>> m_EntityComponentsMap;

	bool checkIfEntityExists(uint32_t entityId);
};
