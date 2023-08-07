#include "Scene.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/MeshComponent.h"

Scene::Scene()
    : m_Id(ECSRegistry::GetInstance().CreateNewEntityId())
{}

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
    ECSRegistry::GetInstance().AddComponent<MeshComponent>(object->GetId());
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
    if (object->GetParentEntityId() != -1)
        ECSRegistry::GetInstance().GetEntity<SceneObject>(object->GetParentEntityId())->RemoveChildEntity(sceneObjectId);

    if(std::find(m_SceneObjectIds.begin(), m_SceneObjectIds.end(), sceneObjectId) != m_SceneObjectIds.end())
        m_SceneObjectIds.erase(std::remove(m_SceneObjectIds.begin(), m_SceneObjectIds.end(), sceneObjectId));
}

uint32_t Scene::AddSceneDirectionalLight()
{
    Ref<Entity> object = ECSRegistry::GetInstance().CreateEntity<DirectionalLightObject>();
    m_SceneLightIds.push_back(object->GetId());
    return object->GetId();
}

uint32_t Scene::AddPointLight()
{
    Ref<Entity> object = ECSRegistry::GetInstance().CreateEntity<PointLightObject>();
    m_SceneLightIds.push_back(object->GetId());
    return object->GetId();
}

void Scene::RemoveSceneLight(uint32_t sceneLightId)
{
    ECSRegistry::GetInstance().RemoveEntity(sceneLightId);

    if (std::find(m_SceneLightIds.begin(), m_SceneLightIds.end(), sceneLightId) != m_SceneLightIds.end())
        m_SceneLightIds.erase(std::remove(m_SceneLightIds.begin(), m_SceneLightIds.end(), sceneLightId));
}

uint32_t Scene::AddCamera(const Ref<Camera> cameraPtr)
{
    Ref<Entity> object = ECSRegistry::GetInstance().CreateEntity<CameraObject>();
    std::static_pointer_cast<CameraObject>(object)->SetCameraPtr(cameraPtr);
    m_CameraId = object->GetId();
    return object->GetId();
}
