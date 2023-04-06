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

void Scene::RemoveSceneObject(uint32_t sceneObjectId)
{
    Ref<Entity> object = ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);
    if (object->GetChildEntities().size() > 0)
    {
        auto childEntities = object->GetChildEntities();
        for (auto& child : childEntities)
        {
            uint32_t childId = child->GetId();
            RemoveSceneObject(childId);
            object->RemoveChildEntity(childId);
        }
    }
    ECSRegistry::GetInstance().RemoveEntity(sceneObjectId);
    if(std::find(m_SceneObjectIds.begin(), m_SceneObjectIds.end(), sceneObjectId) != m_SceneObjectIds.end())
        m_SceneObjectIds.erase(std::remove(m_SceneObjectIds.begin(), m_SceneObjectIds.end(), sceneObjectId));
}
