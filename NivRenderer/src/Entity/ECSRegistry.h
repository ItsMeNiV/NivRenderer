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

	void RemoveEntity(const uint32_t entityId);
    void ClearRegistry();
    void RemoveComponent(const uint32_t entityId, const uint32_t componentId);
	std::vector<Ref<Component>> GetAllComponents(const uint32_t entityId);

	template<typename T>
    Ref<T> AddComponent(const uint32_t entityId);

	template<typename T>
    Ref<T> CreateEntity(const int32_t entityIdParent = -1);

	template<typename T>
    Ref<T> GetEntity(uint32_t entityId);

	template<typename T>
    Ref<T> GetComponent(const uint32_t entityId);

private:
    ECSRegistry() = default;

	std::unordered_map<uint32_t, std::pair<Ref<Entity>, std::vector<Ref<Component>>>> m_EntityComponentsMap;
};

#include "Entity/ECSRegistry.tpp" //Template implementations
