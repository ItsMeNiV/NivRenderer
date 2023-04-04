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

	void AddEntity(Ref<Entity> entity);
	void RemoveEntity(uint32_t entityId);
	void AddComponent(uint32_t entityId, Ref<Component> component);

	template<typename T>
	Ref<T> GetComponent(uint32_t entityId)
	{
		bool found = false;
		for (Ref<Entity> e : m_Entities)
		{
			if (e->GetId() == entityId)
				found = true;
		}

		if (!found)
		{
			std::cerr << "Entity with ID " << std::to_string(entityId) << " not found!" << std::endl;
			return nullptr;
		}

		for (const Ref<Component> c : m_EntityComponentsMap[entityId])
		{
			Ref<T> returnPtr = std::static_pointer_cast<T>(c);
			if (returnPtr)
				return returnPtr;
		}

		return nullptr;
	}

private:
	ECSRegistry() {}

	std::vector<Ref<Entity>> m_Entities;
	std::unordered_map<uint32_t, std::vector<Ref<Component>>> m_EntityComponentsMap;
};
