#include "Scene.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/TransformComponent.h"

Scene::Scene()
{
}

Scene::~Scene()
{
    for (uint32_t id : m_SceneObjectIds)
    {
        ECSRegistry::GetInstance().RemoveEntity(id);
    }
}

uint32_t Scene::AddSceneObject(int32_t parentObjectId)
{
    Ref<Entity> object = ECSRegistry::GetInstance().CreateEntity<SceneObject>(parentObjectId);
    if(parentObjectId == -1)
        m_SceneObjectIds.push_back(object->GetId());
    ECSRegistry::GetInstance().AddComponent<TransformComponent>(object->GetId());
    return object->GetId();
}
