#include "NewScene.h"

#include "IdManager.h"
#include "Entity/NewECSRegistry.h"
#include "Entity/Assets/AssetManager.h"
#include "Util/Math.h"

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
    std::vector<uint32_t> childIdsToDelete;
    if (auto hierarchyElement = GetSceneHierarchyElementById(sceneObjectId))
    {
        // Clear Elem from Parent "ChildList"
        if (hierarchyElement->parentId != UINT32_MAX)
        {
            if(const auto parentElem = GetSceneHierarchyElementById(hierarchyElement->parentId))
                std::erase(parentElem->childIds, hierarchyElement);
        }

        // Remember which childIds to delete
        childIdsToDelete.insert(childIdsToDelete.end(), hierarchyElement->childIds.begin(),
                                hierarchyElement->childIds.end());

        // Delete HierarchyElement
        std::erase(m_SceneHierarchy, *hierarchyElement);
    }
    for (const auto id : childIdsToDelete)
        RemoveSceneObject(id);
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

void NewScene::LoadModel(uint32_t sceneObjectId)
{
    if (const auto meshComponent =
            NewECSRegistry::GetInstance().GetComponent<NewComponents::MeshComponent>(sceneObjectId))
    {
        NewECSRegistry::GetInstance().RemoveComponent<NewComponents::MeshComponent>(sceneObjectId);
    }
    if (const auto materialComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::MaterialComponent>(sceneObjectId))
    {
        NewECSRegistry::GetInstance().RemoveComponent<NewComponents::MaterialComponent>(sceneObjectId);
    }
    if (!NewECSRegistry::GetInstance().GetComponent<NewComponents::TransformComponent>(sceneObjectId))
    {
        NewECSRegistry::GetInstance().AddComponent<NewComponents::TransformComponent>(sceneObjectId);
    }

    const auto sceneObjectComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::SceneObjectComponent>(sceneObjectId);
    const auto tagComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::TagComponent>(sceneObjectId);
    const auto model = AssetManager::GetInstance().LoadModel(sceneObjectComponent->modelPath);
    tagComponent->name = model->name;
    for (auto& subModel : model->subModels)
        createChildSceneObjectFromSubModel(subModel, GetId());
}

void NewScene::SetSkyboxTexturePathsFromFolder()
{
    // Assumes files are named "WHATEVER_Right.WHATEVER", "WHATEVER_Left.WHATEVER", etc.
    // Automatically populates Texture paths in the following order: Right, Left, Top, Bottom, Front, Back

    const auto skyboxComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::SkyboxComponent>(m_SkyboxId);

    if (skyboxComponent->textureFolder.back() != '/')
        skyboxComponent->textureFolder += '/';

    namespace fs = std::filesystem;
    for (const auto& entry : fs::directory_iterator(skyboxComponent->textureFolder))
    {
        std::string filename = entry.path().filename().string();
        std::cmatch m;
        if (std::regex_match(filename.c_str(), m, std::regex(R"(^.*_[R,r]ight..*$)")))
            skyboxComponent->texturePaths[0] = entry.path().string();
        if (std::regex_match(filename.c_str(), m, std::regex(R"(^.*_[L,l]eft..*$)")))
            skyboxComponent->texturePaths[1] = entry.path().string();
        if (std::regex_match(filename.c_str(), m, std::regex(R"(^.*_[T,t]op..*$)")))
            skyboxComponent->texturePaths[2] = entry.path().string();
        if (std::regex_match(filename.c_str(), m, std::regex(R"(^.*_[B,b]ottom..*$)")))
            skyboxComponent->texturePaths[3] = entry.path().string();
        if (std::regex_match(filename.c_str(), m, std::regex(R"(^.*_[F,f]ront..*$)")))
            skyboxComponent->texturePaths[4] = entry.path().string();
        if (std::regex_match(filename.c_str(), m, std::regex(R"(^.*_[B,b]ack..*$)")))
            skyboxComponent->texturePaths[5] = entry.path().string();
    }

    bool allPathsSet = true;
    for (auto& path : skyboxComponent->texturePaths)
    {
        allPathsSet &= !path.empty();
    }
    if (allPathsSet)
        LoadSkyboxTextures();
}

void NewScene::LoadSkyboxTextures() const
{
    const auto skyboxComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::SkyboxComponent>(m_SkyboxId);

    uint8_t i = 0;
    for (auto& path : skyboxComponent->texturePaths)
    {
        if (!path.empty())
            skyboxComponent->textureAssets[i] = AssetManager::GetInstance().LoadTexture(path, skyboxComponent->flipTextures);
        i++;
    }
}

nlohmann::ordered_json NewScene::SerializeObject()
{
    //TODO
    /*
    using namespace nlohmann;
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
     */
    return nlohmann::ordered_json();
}

void NewScene::DeSerializeObject(nlohmann::json jsonObject)
{
    //TODO
    /*
    using namespace nlohmann;
    json sceneSettings = jsonObject["SceneSettings"];
    m_SceneSettings.visualizeLights = sceneSettings["VisualizeLights"];
    m_SceneSettings.animateDirectionalLight = sceneSettings["AnimateDirectionalLight"];
    m_SceneSettings.renderResolution = {sceneSettings["RenderResolution"]["x"], sceneSettings["RenderResolution"]["y"]};
    m_SceneSettings.tempRenderResolution = {sceneSettings["RenderResolution"]["x"],
                                            sceneSettings["RenderResolution"]["y"]};
    m_SceneSettings.shadowmapResolution = {sceneSettings["ShadowmapResolution"]["x"],
                                           sceneSettings["ShadowmapResolution"]["y"]};
    m_SceneSettings.tempShadowmapResolution = {sceneSettings["ShadowmapResolution"]["x"],
                                               sceneSettings["ShadowmapResolution"]["y"]};
    m_SceneSettings.sampleCount = sceneSettings["SampleCount"];

    json textureAssets = jsonObject["Assets"]["TextureAssets"];
    for (json texture : textureAssets)
    {
        auto textureAsset = CreateScope<TextureAsset>(texture["Id"], texture["InternalPath"], texture["FlipVertical"],
                                                      texture["LoadOnlyOneChannel"], texture["ChannelIndex"]);
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
            ECSRegistry::GetInstance()
                .GetEntity<DirectionalLightObject>(directionalLightId)
                ->DeSerializeObject(sceneLight);
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
    */
}

void NewScene::createChildSceneObjectFromSubModel(const SubModel& subModel, const uint32_t parentId)
{
    if (!subModel.mesh && subModel.subModels.empty())
        return; // Empty node, not interesting for us

    const uint32_t subObjectId = AddEmptySceneObject(parentId);
    const auto subObject = NewECSRegistry::GetInstance().GetComponent<NewComponents::SceneObjectComponent>(subObjectId);
    const auto transform = NewECSRegistry::GetInstance().AddComponent<NewComponents::TransformComponent>(subObjectId);
    const auto tag = NewECSRegistry::GetInstance().GetComponent<NewComponents::TagComponent>(subObjectId);

    if (subModel.mesh)
    {
        const auto meshComponent = NewECSRegistry::GetInstance().AddComponent<NewComponents::MeshComponent>(subObjectId);
        meshComponent->meshAsset = subModel.mesh;
        const std::string meshPath = subModel.mesh->GetPath();
        meshComponent->path = meshPath;
    }
    if (subModel.material)
    {
        const auto materialComponent = NewECSRegistry::GetInstance().AddComponent<NewComponents::MaterialComponent>(subObjectId);
        materialComponent->materialAsset = subModel.material;
    }
    tag->name = subModel.name;

    Math::DecomposeMatrix(glm::value_ptr(subModel.modelMatrix), glm::value_ptr(transform->scale),
                          glm::value_ptr(transform->rotation), glm::value_ptr(transform->position));

    for (auto& nextSubModel : subModel.subModels)
        createChildSceneObjectFromSubModel(nextSubModel, subObjectId);
}

SceneHierarchyElement* NewScene::GetSceneHierarchyElementById(uint32_t id)
{
    if (const auto it = std::ranges::find_if(m_SceneHierarchy, [&id](const SceneHierarchyElement& elem) { return elem.entityId == id; }); it != m_SceneHierarchy.end())
    {
        auto& elem = *it;
        return &elem;
    }

    return nullptr;
}
