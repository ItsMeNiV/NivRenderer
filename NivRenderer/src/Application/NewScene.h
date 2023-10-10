#pragma once
#include "OpenGLStarter.h"
#include "nlohmann/json.hpp"

struct SubModel;

struct NewSceneSettings
{
    bool visualizeLights;
    bool animateDirectionalLight;
    glm::ivec2 renderResolution;
    glm::ivec2 tempRenderResolution;
    glm::ivec2 shadowmapResolution;
    glm::ivec2 tempShadowmapResolution;
    uint32_t sampleCount;
};

struct SceneHierarchyElement
{
    uint32_t parentId = UINT32_MAX;
    uint32_t entityId = UINT32_MAX;
    std::vector<uint32_t> childIds;
};

class NewScene
{
public:
    NewScene();
    ~NewScene();

    uint32_t AddSceneObject(uint32_t parentObjectId = UINT32_MAX);
    uint32_t AddEmptySceneObject(uint32_t parentObjectId = UINT32_MAX);
    void RemoveSceneObject(uint32_t sceneObjectId);
    void RemoveSkyboxObject();
    void RemovePointLight(uint32_t pointLightId);
    void RemoveDirectionalLight();
    void RemoveMaterialAsset(uint32_t materialAssetId) const;
    uint32_t AddDirectionalLight();
    uint32_t AddPointLight();
    uint32_t AddSkybox();
    static uint32_t AddMaterialAsset();
    uint32_t AddCamera(Camera* cameraPtr);

    void LoadModel(uint32_t sceneObjectId);
    void SetSkyboxTexturePathsFromFolder();
    void LoadSkyboxTextures() const;

    const uint32_t GetId() const { return m_Id; }
    const std::vector<uint32_t>& GetSceneObjectIds() const { return m_SceneObjectIds; }
    const std::vector<SceneHierarchyElement>& GetSceneHierarchy() const { return m_SceneHierarchy; }
    const uint32_t GetDirectionalLightId() const { return m_DirectionalLightId; }
    const std::vector<uint32_t>& GetPointLightIds() const { return m_PointLightIds; }
    const uint32_t GetSkyboxObjectId() const { return m_SkyboxId; }
    const std::vector<uint32_t>& GetCameraIds() const { return m_CameraIds; }
    const uint32_t GetActiveCameraId() const { return m_ActiveCameraId; }
    void SetActiveCamera(uint32_t cameraId) { m_ActiveCameraId = cameraId; }
    NewSceneSettings& GetSceneSettings() { return m_SceneSettings; }
    const bool HasDirectionalLight() const { return m_HasDirectionalLight; }
    const bool HasSkybox() const { return m_HasSkybox; }

    SceneHierarchyElement* GetSceneHierarchyElementById(uint32_t id);

    nlohmann::ordered_json SerializeObject();
    void DeSerializeObject(nlohmann::json jsonObject);

private:
    uint32_t m_Id;
    std::vector<SceneHierarchyElement> m_SceneHierarchy;
    std::vector<uint32_t> m_SceneObjectIds;
    uint32_t m_DirectionalLightId;
    std::vector<uint32_t> m_PointLightIds;
    std::vector<uint32_t> m_CameraIds;
    uint32_t m_ActiveCameraId;
    uint32_t m_SkyboxId;
    NewSceneSettings m_SceneSettings;
    bool m_HasDirectionalLight;
    bool m_HasSkybox;

    void createChildSceneObjectFromSubModel(const SubModel& subModel, const uint32_t parentId);
};