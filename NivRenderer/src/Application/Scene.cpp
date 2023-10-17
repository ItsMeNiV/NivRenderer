#include "Scene.h"

#include "IdManager.h"
#include "Entity/ECSRegistry.h"
#include "Assets/NewAssetManager.h"
#include "Util/Math.h"

Scene::Scene() :
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

Scene::~Scene() = default;

uint32_t Scene::AddSceneObject(uint32_t parentObjectId)
{
    NewAssetManager& am = NewAssetManager::GetInstance(); // Create Instance first so the default stuff is loaded on Startup
    const auto& entity = ECSRegistry::GetInstance().CreateEntity(this);
    SceneObjectComponent component;
    ECSRegistry::GetInstance().AddComponent<SceneObjectComponent>(entity.id, component);
    TagComponent tagComponent = {std::string("SceneObject (") + std::to_string(entity.id) + std::string(")")};
    ECSRegistry::GetInstance().AddComponent<TagComponent>(entity.id, tagComponent);

    TransformComponent transformComponent;
    ECSRegistry::GetInstance().AddComponent<TransformComponent>(entity.id, transformComponent);
    MeshComponent meshComponent = {"default", am.GetMesh("default")};
    ECSRegistry::GetInstance().AddComponent<MeshComponent>(entity.id, meshComponent);
    MaterialComponent materialComponent = {am.GetMaterial("Default")};
    ECSRegistry::GetInstance().AddComponent<MaterialComponent>(entity.id, materialComponent);

    m_SceneObjectIds.push_back(entity.id);
    m_SceneHierarchy.push_back({parentObjectId, entity.id, std::vector<uint32_t>()});
    if (parentObjectId != UINT32_MAX)
        GetSceneHierarchyElementById(parentObjectId)->childIds.push_back(entity.id);

    return entity.id;
}

uint32_t Scene::AddEmptySceneObject(uint32_t parentObjectId)
{
    const auto& entity = ECSRegistry::GetInstance().CreateEntity(this);
    SceneObjectComponent component;
    ECSRegistry::GetInstance().AddComponent<SceneObjectComponent>(entity.id, component);
    TagComponent tagComponent = {std::string("SceneObject (") + std::to_string(entity.id) + std::string(")")};
    ECSRegistry::GetInstance().AddComponent<TagComponent>(entity.id, tagComponent);

    m_SceneObjectIds.push_back(entity.id);
    m_SceneHierarchy.push_back({parentObjectId, entity.id, std::vector<uint32_t>()});
    if (parentObjectId != UINT32_MAX)
        GetSceneHierarchyElementById(parentObjectId)->childIds.push_back(entity.id);

    return entity.id;
}

void Scene::RemoveSceneObject(uint32_t sceneObjectId)
{
    ECSRegistry::GetInstance().RemoveEntity(sceneObjectId);
    std::erase(m_SceneObjectIds, sceneObjectId);
    std::vector<uint32_t> childIdsToDelete;
    if (const auto hierarchyElement = GetSceneHierarchyElementById(sceneObjectId))
    {
        // Clear Elem from Parent "ChildList"
        if (hierarchyElement->parentId != UINT32_MAX)
        {
            if(const auto parentElem = GetSceneHierarchyElementById(hierarchyElement->parentId))
                std::erase(parentElem->childIds, hierarchyElement->entityId);
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

void Scene::RemoveSkyboxObject()
{
    if (!m_HasSkybox)
        return;

    ECSRegistry::GetInstance().RemoveEntity(m_SkyboxId);
    m_HasSkybox = false;
    m_SkyboxId = UINT32_MAX;
}

void Scene::RemovePointLight(uint32_t pointLightId)
{
    ECSRegistry::GetInstance().RemoveEntity(pointLightId);
    std::erase(m_PointLightIds, pointLightId);
}

void Scene::RemoveDirectionalLight()
{
    if (!m_HasDirectionalLight)
        return;

    ECSRegistry::GetInstance().RemoveEntity(m_DirectionalLightId);
    m_HasDirectionalLight = false;
    m_DirectionalLightId = UINT32_MAX;
}

void Scene::RemoveMaterialAsset(uint32_t materialAssetId) const
{
    // Remove Material from all SceneObjects that use it (Set Material back to Default)
    const auto defaultMaterial = NewAssetManager::GetInstance().GetMaterial("Default");
    for (const auto entity : ECSRegistry::GetInstance().GetAllEntitiesWithComponentType<MaterialComponent>())
    {
        const auto materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(entity->id);
        const auto sceneObjectComponent = ECSRegistry::GetInstance().GetComponent<SceneObjectComponent>(entity->id);
        if (materialComponent->materialAsset->id == materialAssetId)
        {
            materialComponent->materialAsset = defaultMaterial;
            sceneObjectComponent->dirtyFlag = true;
        }
    }

    // Remove Material itself
    NewAssetManager::GetInstance().RemoveMaterial(materialAssetId);
}

uint32_t Scene::AddDirectionalLight()
{
    if (m_HasDirectionalLight)
        return UINT32_MAX;

    const auto& entity = ECSRegistry::GetInstance().CreateEntity(this);
    DirectionalLightComponent component;
    ECSRegistry::GetInstance().AddComponent<DirectionalLightComponent>(entity.id, component);
    TagComponent tagComponent = {"Directional Light"};
    ECSRegistry::GetInstance().AddComponent<TagComponent>(entity.id, tagComponent);

    m_DirectionalLightId = entity.id;
    m_HasDirectionalLight = true;

    return entity.id;
}

uint32_t Scene::AddPointLight()
{
    const auto& entity = ECSRegistry::GetInstance().CreateEntity(this);
    PointLightComponent component;
    ECSRegistry::GetInstance().AddComponent<PointLightComponent>(entity.id, component);
    TagComponent tagComponent = {std::string("Point Light (") + std::to_string(entity.id) +
                                                std::string(")")};
    ECSRegistry::GetInstance().AddComponent<TagComponent>(entity.id, tagComponent);

    m_PointLightIds.push_back(entity.id);

    return entity.id;
}

uint32_t Scene::AddSkybox()
{
    if (m_HasSkybox)
        return UINT32_MAX;

    const auto& entity = ECSRegistry::GetInstance().CreateEntity(this);
    SkyboxComponent component;
    ECSRegistry::GetInstance().AddComponent<SkyboxComponent>(entity.id, component);
    TagComponent tagComponent = {"Skybox"};
    ECSRegistry::GetInstance().AddComponent<TagComponent>(entity.id, tagComponent);

    m_SkyboxId = entity.id;
    m_HasSkybox = true;

    return entity.id;
}

uint32_t Scene::AddMaterialAsset()
{
    const auto materialAsset = NewAssetManager::GetInstance().CreateMaterial();
    return materialAsset->id;
}

uint32_t Scene::AddCamera(Camera* cameraPtr)
{
    const auto& entity = ECSRegistry::GetInstance().CreateEntity(this);
    CameraComponent component = {cameraPtr};
    ECSRegistry::GetInstance().AddComponent<CameraComponent>(entity.id, component);
    TagComponent tagComponent = {"Camera"};
    ECSRegistry::GetInstance().AddComponent<TagComponent>(entity.id, tagComponent);

    m_CameraIds.push_back(entity.id);
    if (m_ActiveCameraId == UINT32_MAX)
        m_ActiveCameraId = entity.id;

    return entity.id;
}

void Scene::LoadModel(uint32_t sceneObjectId)
{
    if (const auto meshComponent =
        ECSRegistry::GetInstance().GetComponent<MeshComponent>(sceneObjectId))
    {
        ECSRegistry::GetInstance().RemoveComponent<MeshComponent>(sceneObjectId);
    }
    if (const auto materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(sceneObjectId))
    {
        ECSRegistry::GetInstance().RemoveComponent<MaterialComponent>(sceneObjectId);
    }
    if (!ECSRegistry::GetInstance().GetComponent<TransformComponent>(sceneObjectId))
    {
        ECSRegistry::GetInstance().AddComponent<TransformComponent>(sceneObjectId);
    }

    const auto sceneObjectComponent = ECSRegistry::GetInstance().GetComponent<SceneObjectComponent>(sceneObjectId);
    const auto tagComponent = ECSRegistry::GetInstance().GetComponent<TagComponent>(sceneObjectId);
    const auto model = NewAssetManager::GetInstance().LoadModel(sceneObjectComponent->modelPath);
    tagComponent->name = model->name;
    for (auto& subModel : model->subModels)
        createChildSceneObjectFromSubModel(subModel, sceneObjectId);
    delete model;
}

void Scene::LoadMesh(uint32_t sceneObjectId)
{
    const auto meshComponent = ECSRegistry::GetInstance().GetComponent<MeshComponent>(sceneObjectId);
    meshComponent->meshAsset = NewAssetManager::GetInstance().LoadMesh(meshComponent->path);
}

void Scene::SetSkyboxTexturePathsFromFolder() const
{
    // Assumes files are named "WHATEVER_Right.WHATEVER", "WHATEVER_Left.WHATEVER", etc.
    // Automatically populates Texture paths in the following order: Right, Left, Top, Bottom, Front, Back

    const auto skyboxComponent = ECSRegistry::GetInstance().GetComponent<SkyboxComponent>(m_SkyboxId);

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

void Scene::LoadSkyboxTextures() const
{
    const auto skyboxComponent = ECSRegistry::GetInstance().GetComponent<SkyboxComponent>(m_SkyboxId);

    uint8_t i = 0;
    for (auto& path : skyboxComponent->texturePaths)
    {
        if (!path.empty())
            skyboxComponent->textureAssets[i] =
                NewAssetManager::GetInstance().LoadTexture(path, skyboxComponent->flipTextures);
        i++;
    }
}

nlohmann::ordered_json Scene::SerializeObject()
{
    using namespace nlohmann;
    ordered_json scene;
    scene["Id"] = m_Id;
    scene["SceneSettings"] = {
        {"VisualizeLights", m_SceneSettings.visualizeLights},
        {"AnimateDirectionalLight", m_SceneSettings.animateDirectionalLight},
        {"RenderResolution", {{"x", m_SceneSettings.renderResolution.x}, {"y", m_SceneSettings.renderResolution.y}}},
        {"ShadowmapResolution", {{"x", m_SceneSettings.shadowmapResolution.x}, {"y", m_SceneSettings.shadowmapResolution.y}}},
        {"SampleCount", m_SceneSettings.sampleCount}
    };

    scene["CurrentId"] = IdManager::GetInstance().GetCurrentId();

    scene["SceneObjects"] = json::array();
    scene["PointLights"] = json::array();

    uint32_t i = 0;
    for (const auto& hierarchyElement : m_SceneHierarchy)
    {
        const auto id = hierarchyElement.entityId;
        const auto sceneObjectComponent = ECSRegistry::GetInstance().GetComponent<SceneObjectComponent>(id);
        const auto tagComponent = ECSRegistry::GetInstance().GetComponent<TagComponent>(id);
        const auto transformComponent = ECSRegistry::GetInstance().GetComponent<TransformComponent>(id);
        const auto materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(id);
        const auto meshComponent = ECSRegistry::GetInstance().GetComponent<MeshComponent>(id);

        ordered_json sceneObjectJson;
        sceneObjectJson["Id"] = id;
        sceneObjectJson["ParentId"] = hierarchyElement.parentId;

        sceneObjectJson["ModelPath"] = sceneObjectComponent->modelPath;
        sceneObjectJson["Components"] = json::array();
        sceneObjectJson["Components"][0]["Type"] = "TagComponent";
        sceneObjectJson["Components"][0]["Name"] = tagComponent->name;

        if (transformComponent)
        {
            ordered_json transformJson;
            transformJson["Type"] = "TransformComponent";
            transformJson["Position"] = {
                {"x", transformComponent->position.x},
                {"y", transformComponent->position.y},
                {"z", transformComponent->position.z}
            };
            transformJson["Scale"] = {
                {"x", transformComponent->scale.x},
                {"y", transformComponent->scale.y},
                {"z", transformComponent->scale.z}
            };
            transformJson["Rotation"] = {
                {"x", transformComponent->rotation.x},
                {"y", transformComponent->rotation.y},
                {"z", transformComponent->rotation.z}
            };
            transformJson["DegRotation"] = {
                {"x", transformComponent->degRotation.x},
                {"y", transformComponent->degRotation.y},
                {"z", transformComponent->degRotation.z}
            };

            sceneObjectJson["Components"][1] = transformJson;
        }

        if (materialComponent)
        {
            ordered_json materialJson;
            materialJson["Type"] = "MaterialComponent";
            materialJson["MaterialAssetId"] = materialComponent->materialAsset->id;

            sceneObjectJson["Components"][2] = materialJson;
        }

        if (meshComponent)
        {
            ordered_json meshJson;
            meshJson["Type"] = "MeshComponent";
            meshJson["Path"] = meshComponent->path;
            meshJson["MeshAssetId"] = meshComponent->meshAsset->id;

            sceneObjectJson["Components"][3] = meshJson;
        }

        scene["SceneObjects"][i] = sceneObjectJson;

        i++;
    }

    if (m_HasDirectionalLight)
    {
        scene["DirectionalLight"] = json::object();
        ordered_json directionalLightJson;
        const auto directionalLightComponent = ECSRegistry::GetInstance().GetComponent<DirectionalLightComponent>(m_DirectionalLightId);
        directionalLightJson["Id"] = m_DirectionalLightId;
        directionalLightJson["LightColor"] = {
            {"r", directionalLightComponent->lightColor.r},
            {"g", directionalLightComponent->lightColor.g},
            {"b", directionalLightComponent->lightColor.b}
        };
        directionalLightJson["Direction"] = {
            {"x", directionalLightComponent->direction.x},
            {"y", directionalLightComponent->direction.y},
            {"z", directionalLightComponent->direction.z}
        };

        scene["DirectionalLight"] = directionalLightJson;
    }

    i = 0;
    for (const auto& id : m_PointLightIds)
    {
        ordered_json pointLightJson;
        const auto pointLightComponent = ECSRegistry::GetInstance().GetComponent<PointLightComponent>(id);
        pointLightJson["Id"] = id;
        pointLightJson["Strength"] = pointLightComponent->strength;
        pointLightJson["LightColor"] = {
            {"r", pointLightComponent->lightColor.r},
            {"g", pointLightComponent->lightColor.g},
            {"b", pointLightComponent->lightColor.b}
        };
        pointLightJson["Position"] = {
            {"x", pointLightComponent->position.x},
            {"y", pointLightComponent->position.y},
            {"z", pointLightComponent->position.z}
        };

        scene["PointLights"][i] = pointLightJson;
        i++;
    }

    if (m_HasSkybox)
    {
        ordered_json skyboxJson;
        const auto skyboxComponent = ECSRegistry::GetInstance().GetComponent<SkyboxComponent>(m_SkyboxId);
        skyboxJson["Id"] = m_SkyboxId;
        skyboxJson["TextureFolder"] = skyboxComponent->textureFolder;
        skyboxJson["FlipTextures"] = skyboxComponent->flipTextures;
        skyboxJson["Textures"] = json::array();
        for (uint32_t j = 0; j < 6; j++)
        {
            ordered_json texture;
            texture["Path"] = skyboxComponent->texturePaths[j];
            texture["AssetId"] = skyboxComponent->textureAssets[j]->id;
            skyboxJson["Textures"][j] = texture;
        }

        scene["Skybox"] = skyboxJson;
    }

    /* TODO: Separate Serialization for Assets
    scene["Assets"]["MaterialAssets"] = json::array();
    i = 0;
    for (const auto& m : NewAssetManager::GetInstance().GetMaterials())
    {
        scene["Assets"]["MaterialAssets"][i] = m.second->SerializeObject();
        i++;
    }

    scene["Assets"]["MeshAssets"] = json::array();
    i = 0;
    for (const auto& m : NewAssetManager::GetInstance().GetMeshes())
    {
        ordered_json mesh = m.second->SerializeObject();
        mesh["Path"] = m.first;
        scene["Assets"]["MeshAssets"][i] = mesh;
        i++;
    }

    scene["Assets"]["TextureAssets"] = json::array();
    i = 0;
    for (const auto& t : NewAssetManager::GetInstance().GetTextures())
    {
        ordered_json texture = t.second->SerializeObject();
        texture["Path"] = t.first;
        scene["Assets"]["TextureAssets"][i] = texture;
        i++;
    }
    */

    return scene;
}

void Scene::DeSerializeObject(nlohmann::json jsonObject)
{
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

    /* TODO: Separate Deserialization of Assets
    json textureAssets = jsonObject["Assets"]["TextureAssets"];
    for (json texture : textureAssets)
    {
        auto textureAsset = CreateScope<TextureAsset>(texture["Id"], texture["InternalPath"], texture["FlipVertical"],
                                                      texture["LoadOnlyOneChannel"], texture["ChannelIndex"]);
        textureAsset->DeSerializeObject(texture);
        NewAssetManager::GetInstance().AddTexture(texture["Path"], std::move(textureAsset));
    }

    json meshAssets = jsonObject["Assets"]["MeshAssets"];
    for (json mesh : meshAssets)
    {
        auto meshAsset = CreateScope<MeshAsset>(mesh["Id"], mesh["InternalPath"]);
        meshAsset->DeSerializeObject(mesh);
        NewAssetManager::GetInstance().AddMesh(mesh["Path"], std::move(meshAsset));
    }

    json materialAssets = jsonObject["Assets"]["MaterialAssets"];
    for (json material : materialAssets)
    {
        auto materialAsset = CreateScope<MaterialAsset>(material["Id"], material["Name"]);
        materialAsset->DeSerializeObject(material);
        NewAssetManager::GetInstance().AddMaterial(std::move(materialAsset));
    }

    json modelAssets = jsonObject["Assets"]["ModelAssets"];
    for (json model : modelAssets)
    {
        auto modelAsset = CreateScope<Model>();
        modelAsset->DeserializeObject(model);
        NewAssetManager::GetInstance().AddModel(model["Path"], std::move(modelAsset));
    }
    */
    
    if (jsonObject.contains("Skybox"))
    {
        json skybox = jsonObject["Skybox"];
        IdManager::GetInstance().SetNextId(skybox["Id"]);
        AddSkybox();
        auto skyboxComponent = ECSRegistry::GetInstance().GetComponent<SkyboxComponent>(m_SkyboxId);
        skyboxComponent->flipTextures = skybox["FlipTextures"];
        skyboxComponent->textureFolder = skybox["TextureFolder"];
        for (uint32_t i = 0; i < 6; i++)
        {
            skyboxComponent->texturePaths[i] = skybox["Textures"][i]["Path"];
            skyboxComponent->textureAssets[i] =
                NewAssetManager::GetInstance().GetTexture(static_cast<uint32_t>(skybox["Textures"][i]["AssetId"]));
        }
    }
    if (jsonObject.contains("DirectionalLight"))
    {
        json directionalLight = jsonObject["DirectionalLight"];
        IdManager::GetInstance().SetNextId(directionalLight["Id"]);
        AddDirectionalLight();
        auto directionalLightComponent = ECSRegistry::GetInstance().GetComponent<DirectionalLightComponent>(m_DirectionalLightId);
        directionalLightComponent->lightColor = {directionalLight["LightColor"]["r"], directionalLight["LightColor"]["g"], directionalLight["LightColor"]["b"]};
        directionalLightComponent->direction = {directionalLight["Direction"]["x"], directionalLight["Direction"]["y"], directionalLight["Direction"]["z"]};
    }
    const json pointLights = jsonObject["PointLights"];
    for (json pointLight : pointLights)
    {
        IdManager::GetInstance().SetNextId(pointLight["Id"]);
        const uint32_t pointLightId = AddPointLight();
        auto pointLightComponent = ECSRegistry::GetInstance().GetComponent<PointLightComponent>(pointLightId);
        pointLightComponent->lightColor = {pointLight["LightColor"]["r"], pointLight["LightColor"]["g"], pointLight["LightColor"]["b"]};
        pointLightComponent->position = {pointLight["Position"]["x"], pointLight["Position"]["y"], pointLight["Position"]["z"]};
        pointLightComponent->strength = pointLight["Strength"];
    }

    json sceneObjects = jsonObject["SceneObjects"];
    for (json sceneObject : sceneObjects)
    {
        IdManager::GetInstance().SetNextId(sceneObject["Id"]);
        const uint32_t sceneObjectId = AddEmptySceneObject(sceneObject["ParentId"]);
        const auto sceneObjectComponent = ECSRegistry::GetInstance().GetComponent<SceneObjectComponent>(sceneObjectId);
        sceneObjectComponent->modelPath = sceneObject["ModelPath"];
        for (json component : sceneObject["Components"])
        {
            if (component["Type"] == "TagComponent")
            {
                auto tagComponent = ECSRegistry::GetInstance().GetComponent<TagComponent>(sceneObjectId);
                tagComponent->name = component["Name"];
            }
            else if (component["Type"] == "TransformComponent")
            {
                const TransformComponent transform = {
                    {component["Position"]["x"], component["Position"]["y"], component["Position"]["z"]},
                    {component["Scale"]["x"], component["Scale"]["y"], component["Scale"]["z"]},
                    {component["Rotation"]["x"], component["Rotation"]["y"], component["Rotation"]["z"]},
                    {component["DegRotation"]["x"], component["DegRotation"]["y"], component["DegRotation"]["z"]}
                };
                ECSRegistry::GetInstance().AddComponent<TransformComponent>(sceneObjectId, transform);
            }
            else if (component["Type"] == "MaterialComponent")
            {
                const MaterialComponent material = {
                    NewAssetManager::GetInstance().GetMaterial(static_cast<uint32_t>(component["MaterialAssetId"]))};
                ECSRegistry::GetInstance().AddComponent<MaterialComponent>(sceneObjectId, material);
            }
            else if (component["Type"] == "MeshComponent")
            {
                const MeshComponent mesh = {component["Path"],
                                            NewAssetManager::GetInstance().GetMesh(static_cast<uint32_t>(component["MeshAssetId"]))};
                ECSRegistry::GetInstance().AddComponent<MeshComponent>(sceneObjectId, mesh);
            }
        }
    }

    const uint32_t currentId = jsonObject["CurrentId"];
    IdManager::GetInstance().SetNextId(currentId + 1);
}

void Scene::createChildSceneObjectFromSubModel(const SubModel& subModel, const uint32_t parentId)
{
    if (!subModel.mesh && subModel.subModels.empty())
        return; // Empty node, not interesting for us

    const uint32_t subObjectId = AddEmptySceneObject(parentId);
    ECSRegistry::GetInstance().GetComponent<SceneObjectComponent>(subObjectId);
    const auto transform = ECSRegistry::GetInstance().AddComponent<TransformComponent>(subObjectId);
    const auto tag = ECSRegistry::GetInstance().GetComponent<TagComponent>(subObjectId);

    if (subModel.mesh)
    {
        const auto meshComponent = ECSRegistry::GetInstance().AddComponent<MeshComponent>(subObjectId);
        meshComponent->meshAsset = subModel.mesh;
        const std::string meshPath = subModel.mesh->path;
        meshComponent->path = meshPath;
    }
    if (subModel.material)
    {
        const auto materialComponent = ECSRegistry::GetInstance().AddComponent<MaterialComponent>(subObjectId);
        materialComponent->materialAsset = subModel.material;
    }
    tag->name = subModel.name;

    Math::DecomposeMatrix(glm::value_ptr(subModel.modelMatrix), glm::value_ptr(transform->scale),
                          glm::value_ptr(transform->rotation), glm::value_ptr(transform->position));

    for (auto& nextSubModel : subModel.subModels)
        createChildSceneObjectFromSubModel(nextSubModel, subObjectId);
}

SceneHierarchyElement* Scene::GetSceneHierarchyElementById(uint32_t id)
{
    if (const auto it = std::ranges::find_if(m_SceneHierarchy, [&id](const SceneHierarchyElement& elem) { return elem.entityId == id; }); it != m_SceneHierarchy.end())
    {
        auto& elem = *it;
        return &elem;
    }

    return nullptr;
}
