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

Scene::~Scene() = default;

uint32_t Scene::AddSceneObject(int32_t parentObjectId)
{
    AssetManager& am = AssetManager::GetInstance(); //Create Instance first so the default stuff is loaded on Startup
    const auto object = ECSRegistry::GetInstance().CreateEntity<SceneObject>(parentObjectId);
    if(parentObjectId == -1)
        m_SceneObjectIds.push_back(object->GetId());
    ECSRegistry::GetInstance().AddComponent<TransformComponent>(object->GetId());
    ECSRegistry::GetInstance().AddComponent<MeshComponent>(object->GetId());
    ECSRegistry::GetInstance().AddComponent<MaterialComponent>(object->GetId());
    ECSRegistry::GetInstance().GetComponent<MaterialComponent>(object->GetId())->SetMaterialAsset(am.GetMaterial("Default"));
    return object->GetId();
}

uint32_t Scene::AddEmptySceneObject(int32_t parentObjectId)
{
    AssetManager& am = AssetManager::GetInstance(); // Create Instance first so the default stuff is loaded on Startup
    const auto object = ECSRegistry::GetInstance().CreateEntity<SceneObject>(parentObjectId);
    if (parentObjectId == -1)
        m_SceneObjectIds.push_back(object->GetId());
    return object->GetId();
}

void Scene::RemoveSceneObject(uint32_t sceneObjectId)
{
    ECSRegistry::GetInstance().RemoveEntity(sceneObjectId);
    std::erase(m_SceneObjectIds, sceneObjectId);
}

void Scene::RemoveSkyboxObject()
{
    ECSRegistry::GetInstance().RemoveEntity(m_SkyboxId);
    m_HasSkybox = false;
    m_SkyboxId = UINT32_MAX;
}

uint32_t Scene::AddDirectionalLight()
{
    const auto object = ECSRegistry::GetInstance().CreateEntity<DirectionalLightObject>();
    m_SceneLightIds.push_back(object->GetId());
    m_HasDirectionalLight = true;
    return object->GetId();
}

uint32_t Scene::AddPointLight()
{
    const auto object = ECSRegistry::GetInstance().CreateEntity<PointLightObject>();
    m_SceneLightIds.push_back(object->GetId());
    return object->GetId();
}

uint32_t Scene::AddSkybox()
{
    const auto object = ECSRegistry::GetInstance().CreateEntity<SkyboxObject>();
    m_SkyboxId = object->GetId();
    m_HasSkybox = true;
    return 0;
}

uint32_t Scene::AddMaterialAsset()
{
    const auto materialAsset = AssetManager::GetInstance().CreateMaterial();
    return materialAsset->GetId();
}

void Scene::RemoveSceneLight(uint32_t sceneLightId)
{
    if (ECSRegistry::GetInstance().GetEntity<DirectionalLightObject>(sceneLightId))
        m_HasDirectionalLight = false;

    ECSRegistry::GetInstance().RemoveEntity(sceneLightId);
    std::erase(m_SceneLightIds, sceneLightId);
}

void Scene::RemoveMaterialAsset(uint32_t materialAssetId) const
{
    //Remove Material from all SceneObjects that use it (Set Material back to Default)
    const auto defaultMaterial = AssetManager::GetInstance().GetMaterial("Default");
    for(const uint32_t sceneObjectId : m_SceneObjectIds)
    {
        const auto sceneObject = ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId);
        const auto materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(sceneObjectId);
        if (materialComponent->GetMaterialAsset()->GetId() != materialAssetId)
            continue;

        materialComponent->SetMaterialAsset(defaultMaterial);
        if (const auto model = AssetManager::GetInstance().GetModel(*sceneObject->GetModelPath()))
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

    //Remove Material itself
    AssetManager::GetInstance().RemoveMaterial(materialAssetId);
}

uint32_t Scene::AddCamera(Camera* cameraPtr)
{
    const auto object = ECSRegistry::GetInstance().CreateEntity<CameraObject>();
    object->SetCameraPtr(cameraPtr);
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

    scene["CurrentId"] = IdManager::GetInstance().GetCurrentId();

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

void Scene::DeSerializeObject(json jsonObject)
{
    json sceneSettings = jsonObject["SceneSettings"];
    m_SceneSettings.visualizeLights = sceneSettings["VisualizeLights"];
    m_SceneSettings.animateDirectionalLight = sceneSettings["AnimateDirectionalLight"];
    m_SceneSettings.renderResolution = { sceneSettings["RenderResolution"]["x"], sceneSettings["RenderResolution"]["y"] };
    m_SceneSettings.tempRenderResolution = {sceneSettings["RenderResolution"]["x"], sceneSettings["RenderResolution"]["y"]};
    m_SceneSettings.shadowmapResolution = {sceneSettings["ShadowmapResolution"]["x"], sceneSettings["ShadowmapResolution"]["y"]};
    m_SceneSettings.tempShadowmapResolution = {sceneSettings["ShadowmapResolution"]["x"], sceneSettings["ShadowmapResolution"]["y"]};
    m_SceneSettings.sampleCount = sceneSettings["SampleCount"];

    json textureAssets = jsonObject["Assets"]["TextureAssets"];
    for (json texture : textureAssets)
    {
        auto textureAsset = CreateScope<TextureAsset>(texture["Id"], texture["InternalPath"], texture["FlipVertical"]);
        textureAsset->DeSerializeObject(texture);
        AssetManager::GetInstance().AddTexture(texture["Path"], std::move(textureAsset));
    }

    json meshAssets = jsonObject["Assets"]["MeshAssets"];
    for (json mesh : meshAssets)
    {
        auto meshAsset = CreateScope<MeshAsset>(mesh["Id"], mesh["InternalPath"]);
        meshAsset->DeSerializeObject(mesh);
        AssetManager::GetInstance().AddMesh(mesh["Path"], std::move(meshAsset));
    }

    json materialAssets = jsonObject["Assets"]["MaterialAssets"];
    for (json material : materialAssets)
    {
        auto materialAsset = CreateScope<MaterialAsset>(material["Id"], material["Name"]);
        materialAsset->DeSerializeObject(material);
        AssetManager::GetInstance().AddMaterial(std::move(materialAsset));
    }

    json modelAssets = jsonObject["Assets"]["ModelAssets"];
    for (json model : modelAssets)
    {
        auto modelAsset = CreateScope<Model>();
        modelAsset->DeserializeObject(model);
        AssetManager::GetInstance().AddModel(model["Path"], std::move(modelAsset));
    }

    if (jsonObject.contains("Skybox"))
    {
        json skybox = jsonObject["Skybox"];
        IdManager::GetInstance().SetNextId(skybox["Id"]);
        AddSkybox();
        ECSRegistry::GetInstance().GetEntity<SkyboxObject>(m_SkyboxId)->DeSerializeObject(skybox);
    }

    json sceneLights = jsonObject["SceneLights"];
    for (json sceneLight : sceneLights)
    {
        if (sceneLight["Type"] == "DirectionalLight")
        {
            IdManager::GetInstance().SetNextId(sceneLight["Id"]);
            const uint32_t directionalLightId = AddDirectionalLight();
            ECSRegistry::GetInstance().GetEntity<DirectionalLightObject>(directionalLightId)->DeSerializeObject(sceneLight);
        }
        else if (sceneLight["Type"] == "PointLight")
        {
            IdManager::GetInstance().SetNextId(sceneLight["Id"]);
            const uint32_t pointLightId = AddPointLight();
            ECSRegistry::GetInstance().GetEntity<PointLightObject>(pointLightId)->DeSerializeObject(sceneLight);
        }
    }

    json sceneObjects = jsonObject["SceneObjects"];
    for (json sceneObject : sceneObjects)
    {
        IdManager::GetInstance().SetNextId(sceneObject["Id"]);
        const uint32_t sceneObjectId = AddEmptySceneObject();
        ECSRegistry::GetInstance().GetEntity<SceneObject>(sceneObjectId)->DeSerializeObject(sceneObject);
    }

    const uint32_t currentId = jsonObject["CurrentId"];
    IdManager::GetInstance().SetNextId(currentId + 1);
}
