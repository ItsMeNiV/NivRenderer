#include "NewScene.h"

#include "IdManager.h"
#include "Entity/NewECSRegistry.h"
#include "Entity/Assets/AssetManager.h"

NewScene::NewScene() :
    m_Id(IdManager::GetInstance().CreateNewId()), m_DirectionalLightId(UINT32_MAX), m_ActiveCameraId(UINT32_MAX), m_SkyboxId(UINT32_MAX)
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

NewScene::~NewScene() = default;

uint32_t NewScene::AddSceneObject(uint32_t parentObjectId)
{
    AssetManager& am = AssetManager::GetInstance(); // Create Instance first so the default stuff is loaded on Startup
    const auto& entity = NewECSRegistry::GetInstance().CreateEntity(this);
    NewComponents::SceneObjectComponent component;
    NewECSRegistry::GetInstance().AddComponent<NewComponents::SceneObjectComponent>(entity.id, component);
    NewComponents::TagComponent tagComponent = {std::string("SceneObject (") + std::to_string(entity.id) + std::string(")")};
    NewECSRegistry::GetInstance().AddComponent<NewComponents::TagComponent>(entity.id, tagComponent);

    NewComponents::TransformComponent transformComponent;
    NewECSRegistry::GetInstance().AddComponent<NewComponents::TransformComponent>(entity.id, transformComponent);
    NewComponents::MeshComponent meshComponent;
    NewECSRegistry::GetInstance().AddComponent<NewComponents::MeshComponent>(entity.id, transformComponent);
    NewComponents::MaterialComponent materialComponent = {am.GetMaterial("Default")};
    NewECSRegistry::GetInstance().AddComponent<NewComponents::MaterialComponent>(entity.id, transformComponent);

    m_SceneObjectIds.push_back(entity.id);
    m_SceneHierarchy.push_back({parentObjectId, entity.id, std::vector<uint32_t>()});

    return entity.id;
}

uint32_t NewScene::AddEmptySceneObject(uint32_t parentObjectId)
{
    const auto& entity = NewECSRegistry::GetInstance().CreateEntity(this);
    NewComponents::SceneObjectComponent component;
    NewECSRegistry::GetInstance().AddComponent<NewComponents::SceneObjectComponent>(entity.id, component);
    NewComponents::TagComponent tagComponent = {std::string("SceneObject (") + std::to_string(entity.id) + std::string(")")};
    NewECSRegistry::GetInstance().AddComponent<NewComponents::TagComponent>(entity.id, tagComponent);

    m_SceneObjectIds.push_back(entity.id);
    m_SceneHierarchy.push_back({parentObjectId, entity.id, std::vector<uint32_t>()});

    return entity.id;
}

void NewScene::RemoveSceneObject(uint32_t sceneObjectId)
{
    NewECSRegistry::GetInstance().RemoveEntity(sceneObjectId);
    std::erase(m_SceneObjectIds, sceneObjectId);
    //TODO
}

void NewScene::RemoveSkyboxObject()
{
    if (!m_HasSkybox)
        return;

    NewECSRegistry::GetInstance().RemoveEntity(m_SkyboxId);
    m_HasSkybox = false;
    m_SkyboxId = UINT32_MAX;
}

void NewScene::RemovePointLight(uint32_t pointLightId)
{
    NewECSRegistry::GetInstance().RemoveEntity(pointLightId);
    std::erase(m_PointLightIds, pointLightId);
}

void NewScene::RemoveDirectionalLight()
{
    if (!m_HasDirectionalLight)
        return;

    NewECSRegistry::GetInstance().RemoveEntity(m_DirectionalLightId);
    m_HasDirectionalLight = false;
    m_DirectionalLightId = UINT32_MAX;
}

void NewScene::RemoveMaterialAsset(uint32_t materialAssetId) const
{
    // Remove Material from all SceneObjects that use it (Set Material back to Default)
    const auto defaultMaterial = AssetManager::GetInstance().GetMaterial("Default");
    for (const uint32_t sceneObjectId : m_SceneObjectIds)
    {
        const auto sceneObject = NewECSRegistry::GetInstance().GetEntity(sceneObjectId);
        const auto sceneObjectComponent =
            NewECSRegistry::GetInstance().GetComponent<NewComponents::SceneObjectComponent>(sceneObjectId);
        const auto materialComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::MaterialComponent>(sceneObjectId);
        if (materialComponent->materialAsset->GetId() != materialAssetId)
            continue;

        materialComponent->materialAsset = defaultMaterial;
        if (const auto model = AssetManager::GetInstance().GetModel(sceneObjectComponent->modelPath))
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
        sceneObjectComponent->dirtyFlag = true;
    }

    // Remove Material itself
    AssetManager::GetInstance().RemoveMaterial(materialAssetId);
}

uint32_t NewScene::AddDirectionalLight()
{
    if (m_HasDirectionalLight)
        return UINT32_MAX;

    const auto& entity = NewECSRegistry::GetInstance().CreateEntity(this);
    NewComponents::DirectionalLightComponent component;
    NewECSRegistry::GetInstance().AddComponent<NewComponents::DirectionalLightComponent>(entity.id, component);
    NewComponents::TagComponent tagComponent = {"Directional Light"};
    NewECSRegistry::GetInstance().AddComponent<NewComponents::TagComponent>(entity.id, tagComponent);

    m_DirectionalLightId = entity.id;
    m_HasDirectionalLight = true;

    return entity.id;
}

uint32_t NewScene::AddPointLight()
{
    const auto& entity = NewECSRegistry::GetInstance().CreateEntity(this);
    NewComponents::PointLightComponent component;
    NewECSRegistry::GetInstance().AddComponent<NewComponents::PointLightComponent>(entity.id, component);
    NewComponents::TagComponent tagComponent = {std::string("Point Light (") + std::to_string(entity.id) +
                                                std::string(")")};
    NewECSRegistry::GetInstance().AddComponent<NewComponents::TagComponent>(entity.id, tagComponent);

    m_PointLightIds.push_back(entity.id);

    return entity.id;
}

uint32_t NewScene::AddSkybox()
{
    if (m_HasSkybox)
        return UINT32_MAX;

    const auto& entity = NewECSRegistry::GetInstance().CreateEntity(this);
    NewComponents::SkyboxComponent component;
    NewECSRegistry::GetInstance().AddComponent<NewComponents::DirectionalLightComponent>(entity.id, component);
    NewComponents::TagComponent tagComponent = {"Skybox"};
    NewECSRegistry::GetInstance().AddComponent<NewComponents::TagComponent>(entity.id, tagComponent);

    m_SkyboxId = entity.id;
    m_HasSkybox = true;

    return entity.id;
}

uint32_t NewScene::AddMaterialAsset()
{
    const auto materialAsset = AssetManager::GetInstance().CreateMaterial();
    return materialAsset->GetId();
}

uint32_t NewScene::AddCamera(Camera* cameraPtr)
{
    const auto& entity = NewECSRegistry::GetInstance().CreateEntity(this);
    NewComponents::CameraComponent component = {cameraPtr};
    NewECSRegistry::GetInstance().AddComponent<NewComponents::CameraComponent>(entity.id, component);
    NewComponents::TagComponent tagComponent = {"Camera"};
    NewECSRegistry::GetInstance().AddComponent<NewComponents::TagComponent>(entity.id, tagComponent);

    m_CameraIds.push_back(entity.id);
    if (m_ActiveCameraId == UINT32_MAX)
        m_ActiveCameraId = entity.id;

    return entity.id;
}

nlohmann::ordered_json NewScene::SerializeObject()
{
    return nlohmann::ordered_json();
}

void NewScene::DeSerializeObject(nlohmann::json jsonObject)
{
    //TODO
}
