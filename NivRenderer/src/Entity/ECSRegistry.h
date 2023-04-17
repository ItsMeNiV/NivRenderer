#pragma once
#include "Base.h"
#include "Entity/Entity.h"
#include "Entity/Component.h"

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

	uint32_t CreateNewEntityId();

	template<typename T>
	void AddComponent(uint32_t entityId)
	{
		if (!checkIfEntityExists(entityId))
		{
			std::cerr << "Entity with ID " << std::to_string(entityId) << " not found!" << std::endl;
			return;
		}

		Ref<T> component = CreateRef<T>();

		m_EntityComponentsMapNew[entityId].second.push_back(component);
	}

	template<typename T>
	Ref<T> CreateEntity(int32_t entityIdParent = -1)
	{
		static_assert(std::is_base_of_v<Entity, T>);

		Ref<T> entity = CreateRef<T>(CreateNewEntityId());
		m_EntityComponentsMapNew[entity->GetId()].first = entity;
		m_EntityComponentsMapNew[entity->GetId()].second = std::vector<Ref<Component>>();

		if (entityIdParent != -1)
		{
			m_EntityComponentsMapNew[entityIdParent].first->AddChildEntity(entity);
		}
			
		return entity;
	}

	template<typename T>
	Ref<T> GetEntity(uint32_t entityId)
	{
		auto entity = m_EntityComponentsMapNew[entityId].first;
		Ref<T> returnPtr = std::static_pointer_cast<T>(entity);
		if (returnPtr)
			return returnPtr;

		std::cerr << "Entity with ID " << std::to_string(entityId) << " not found!" << std::endl;
		return nullptr;
	}

	template<typename T>
	Ref<T> GetComponent(uint32_t entityId)
	{
		if (!checkIfEntityExists(entityId))
		{
			std::cerr << "Entity with ID " << std::to_string(entityId) << " not found!" << std::endl;
			return nullptr;
		}

		for (const Ref<Component> c : m_EntityComponentsMapNew[entityId].second)
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

	std::unordered_map<uint32_t, std::pair<Ref<Entity>, std::vector<Ref<Component>>>> m_EntityComponentsMapNew;

	bool checkIfEntityExists(uint32_t entityId);
};
