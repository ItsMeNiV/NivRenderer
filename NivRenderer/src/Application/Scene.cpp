#include "Scene.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/MaterialComponent.h"

Scene::Scene()
    : m_Id(ECSRegistry::GetInstance().CreateNewEntityId()), m_CameraId(UINT32_MAX), m_SkyboxId(UINT32_MAX)
{
    m_SceneSettings.visualizeLights = false;
    m_SceneSettings.animateDirectionalLight = false;
    m_SceneSettings.renderResolution = {1920, 1080};
    m_SceneSettings.sampleCount = 4;
    m_HasDirectionalLight = false;
    m_HasSkybox = false;
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
    const Ref<SceneObject> object = ECSRegistry::GetInstance().CreateEntity<SceneObject>(parentObjectId);
    if(parentObjectId == -1)
        m_SceneObjectIds.push_back(object->GetId());
    ECSRegistry::GetInstance().AddComponent<TransformComponent>(object->GetId());
    ECSRegistry::GetInstance().AddComponent<MeshComponent>(object->GetId());
    ECSRegistry::GetInstance().AddComponent<MaterialComponent>(object->GetId());
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

void Scene::RemoveSkyboxObject()
{
    ECSRegistry::GetInstance().RemoveEntity(m_SkyboxId);
    m_HasSkybox = false;
    m_SkyboxId = UINT32_MAX;
}

uint32_t Scene::AddDirectionalLight()
{
    Ref<Entity> object = ECSRegistry::GetInstance().CreateEntity<DirectionalLightObject>();
    m_SceneLightIds.push_back(object->GetId());
    m_HasDirectionalLight = true;
    return object->GetId();
}

uint32_t Scene::AddPointLight()
{
    Ref<Entity> object = ECSRegistry::GetInstance().CreateEntity<PointLightObject>();
    m_SceneLightIds.push_back(object->GetId());
    return object->GetId();
}

uint32_t Scene::AddSkybox()
{
    Ref<Entity> object = ECSRegistry::GetInstance().CreateEntity<SkyboxObject>();
    m_SkyboxId = object->GetId();
    m_HasSkybox = true;
    return 0;
}

void Scene::RemoveSceneLight(uint32_t sceneLightId)
{
    if (ECSRegistry::GetInstance().GetEntity<DirectionalLightObject>(sceneLightId))
        m_HasDirectionalLight = false;

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
