#include "Scene.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/MaterialComponent.h"

Scene::Scene()
    : m_Id(IdManager::GetInstance().CreateNewId()), m_CameraId(UINT32_MAX), m_SkyboxId(UINT32_MAX)
{
    m_SceneSettings.visualizeLights = false;
    m_SceneSettings.animateDirectionalLight = false;
    m_SceneSettings.renderResolution = {1920, 1080};
    m_SceneSettings.tempRenderResolution = {1920, 1080};
    m_SceneSettings.shadowmapResolution = {1024, 1024};
    m_SceneSettings.tempShadowmapResolution = {1024, 1024};
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
    AssetManager& am = AssetManager::GetInstance(); //Create Instance first so the default stuff is loaded on Startup
    const Ref<SceneObject> object = ECSRegistry::GetInstance().CreateEntity<SceneObject>(parentObjectId);
    if(parentObjectId == -1)
        m_SceneObjectIds.push_back(object->GetId());
    ECSRegistry::GetInstance().AddComponent<TransformComponent>(object->GetId());
    ECSRegistry::GetInstance().AddComponent<MeshComponent>(object->GetId());
    ECSRegistry::GetInstance().AddComponent<MaterialComponent>(object->GetId());
    ECSRegistry::GetInstance().GetComponent<MaterialComponent>(object->GetId())->GetMaterialAsset() = am.GetMaterial("Default");
    return object->GetId();
}

uint32_t Scene::AddEmptySceneObject(int32_t parentObjectId)
{
    AssetManager& am = AssetManager::GetInstance(); // Create Instance first so the default stuff is loaded on Startup
    const Ref<SceneObject> object = ECSRegistry::GetInstance().CreateEntity<SceneObject>(parentObjectId);
    if (parentObjectId == -1)
        m_SceneObjectIds.push_back(object->GetId());
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

uint32_t Scene::AddMaterialAsset()
{
    const Ref<MaterialAsset> materialAsset = AssetManager::GetInstance().CreateMaterial();
    return materialAsset->GetId();
}

void Scene::RemoveSceneLight(uint32_t sceneLightId)
{
    if (ECSRegistry::GetInstance().GetEntity<DirectionalLightObject>(sceneLightId))
        m_HasDirectionalLight = false;

    ECSRegistry::GetInstance().RemoveEntity(sceneLightId);

    if (std::ranges::find(m_SceneLightIds, sceneLightId) != m_SceneLightIds.end())
        m_SceneLightIds.erase(std::ranges::remove(m_SceneLightIds, sceneLightId).begin());
}

void Scene::RemoveMaterialAsset(uint32_t materialAssetId)
{
    //Remove Material from all SceneObjects that use it (Set Material back to Default)
    const auto defaultMaterial = AssetManager::GetInstance().GetMaterial("Default");
    for(const uint32_t sceneObjectId : m_SceneObjectIds)
    {
        const auto sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);
        const auto materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(sceneObjectId);
        if (materialComponent->GetMaterialAsset()->GetId() != materialAssetId)
            continue;

        materialComponent->GetMaterialAsset() = defaultMaterial;
        if (const Ref<Model> model = AssetManager::GetInstance().GetModel(*sceneObject->GetModelPath()))
        {
            auto& subModels = model->subModels;
            while (!subModels.empty())
            {
                for (auto& subModel : subModels)
                {
                    if (subModel.material->GetId() == materialAssetId)
                        subModel.material = defaultMaterial;
                }
            }
        }
        sceneObject->SetDirtyFlag(true);
    }

    //Remove Material itsself
    AssetManager::GetInstance().RemoveMaterial(materialAssetId);
}

uint32_t Scene::AddCamera(const Ref<Camera> cameraPtr)
{
    Ref<Entity> object = ECSRegistry::GetInstance().CreateEntity<CameraObject>();
    std::static_pointer_cast<CameraObject>(object)->SetCameraPtr(cameraPtr);
    m_CameraId = object->GetId();
    return object->GetId();
}

std::vector<std::pair<std::string, Property>> Scene::GetEntityProperties()
{
    std::vector<std::pair<std::string, Property>> returnVector;

    returnVector.push_back({"General Scene Settings", {PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"Visualize Lights", {PropertyType::BOOL, &m_SceneSettings.visualizeLights, [this]() {}}});
    returnVector.push_back({"Animate Directional Light", {PropertyType::BOOL, &m_SceneSettings.animateDirectionalLight, [this]() {}}});
    returnVector.push_back({"Render Resolution", {PropertyType::INT2, glm::value_ptr(m_SceneSettings.tempRenderResolution), [this]() {}}});
    returnVector.push_back({"Shadowmap Resolution",{PropertyType::INT2, glm::value_ptr(m_SceneSettings.tempShadowmapResolution), [this]() {}}});
    returnVector.push_back({"Apply resolution", {PropertyType::BUTTON, nullptr, [this]() {
        m_SceneSettings.renderResolution = m_SceneSettings.tempRenderResolution;
        m_SceneSettings.shadowmapResolution = m_SceneSettings.tempShadowmapResolution;
    }}});
    returnVector.push_back({"Sample count", {PropertyType::INT, &m_SceneSettings.sampleCount, [this]() {}}});

    return returnVector;
}

ordered_json Scene::SerializeObject()
{
    ordered_json scene;
    scene["SceneSettings"] = {
        {"VisualizeLights", m_SceneSettings.visualizeLights},
        {"AnimateDirectionalLight", m_SceneSettings.animateDirectionalLight},
        {"RenderResolution", {{"x", m_SceneSettings.renderResolution.x}, {"y", m_SceneSettings.renderResolution.y}}},
        {"ShadowmapResolution", {{"x", m_SceneSettings.shadowmapResolution.x}, {"y", m_SceneSettings.shadowmapResolution.y}}},
        {"SampleCount", m_SceneSettings.sampleCount}
    };

    scene["SceneObjects"] = json::array();
    scene["SceneLights"] = json::array();
    uint32_t i = 0;
    for (const auto& id : m_SceneObjectIds)
    {
        scene["SceneObjects"][i] = ECSRegistry::GetInstance().GetEntity<SceneObject>(id)->SerializeObject();
        i++;
    }
    i = 0;
    for (const auto& id : m_SceneLightIds)
    {
        scene["SceneLights"][i] = ECSRegistry::GetInstance().GetEntity<LightObject>(id)->SerializeObject();
        i++;
    }
    if (m_HasSkybox)
    {
        scene["Skybox"] = ECSRegistry::GetInstance().GetEntity<SkyboxObject>(m_SkyboxId)->SerializeObject();
    }

    scene["Assets"]["ModelAssets"] = json::array();
    i = 0;
    for(auto& m : AssetManager::GetInstance().GetModels())
    {
        ordered_json model = m.second->SerializeObject();
        model["Path"] = m.first;
        scene["Assets"]["ModelAssets"][i] = model;
        i++;
    }

    scene["Assets"]["MaterialAssets"] = json::array();
    i = 0;
    for (const auto& m : AssetManager::GetInstance().GetMaterials())
    {
        scene["Assets"]["MaterialAssets"][i] = m.second->SerializeObject();
        i++;
    }

    scene["Assets"]["MeshAssets"] = json::array();
    i = 0;
    for (const auto& m : AssetManager::GetInstance().GetMeshes())
    {
        ordered_json mesh = m.second->SerializeObject();
        mesh["Path"] = m.first;
        scene["Assets"]["MeshAssets"][i] = mesh;
        i++;
    }

    scene["Assets"]["TextureAssets"] = json::array();
    i = 0;
    for (const auto& t : AssetManager::GetInstance().GetTextures())
    {
        ordered_json texture = t.second->SerializeObject();
        texture["Path"] = t.first;
        scene["Assets"]["TextureAssets"][i] = texture;
        i++;
    }

    return scene;
}
