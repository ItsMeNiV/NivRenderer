#pragma once
#include "Base.h"
#include "Entity/Entities/SceneObject.h"
#include "Entity/Entities/LightObject.h"
#include "Entity/Entities/CameraObject.h"
#include "Entity/Entities/SkyboxObject.h"
#include "Application/Serialization/Serializable.h"

struct SceneSettings
{
    bool visualizeLights;
    bool animateDirectionalLight;
    glm::ivec2 renderResolution;
    glm::ivec2 tempRenderResolution;
    glm::ivec2 shadowmapResolution;
    glm::ivec2 tempShadowmapResolution;
    uint32_t sampleCount;
};

class Scene : public Serializable
{
public:
    Scene();
    ~Scene();

    uint32_t AddSceneObject(int32_t parentObjectId = -1);
    uint32_t AddEmptySceneObject(int32_t parentObjectId = -1);
    void RemoveSceneObject(uint32_t sceneObjectId);
    void RemoveSkyboxObject();
    void RemoveSceneLight(uint32_t sceneLightId);
    void RemoveMaterialAsset(uint32_t materialAssetId) const;
    uint32_t AddDirectionalLight();
    uint32_t AddPointLight();
    uint32_t AddSkybox();
    static uint32_t AddMaterialAsset();

    uint32_t AddCamera(Camera* cameraPtr);

    const uint32_t GetId() const { return m_Id; }
    const std::vector<uint32_t>& GetSceneObjectIds() const { return m_SceneObjectIds; }
    const std::vector<uint32_t>& GetSceneLightIds() const { return m_SceneLightIds; }
    const uint32_t GetSkyboxObjectId() const { return m_SkyboxId; }
    const uint32_t GetCameraId() const { return m_CameraId; }
    SceneSettings& GetSceneSettings() { return m_SceneSettings; }
    const bool HasDirectionalLight() const { return m_HasDirectionalLight; }
    const bool HasSkybox() const { return m_HasSkybox; }

    std::vector<std::pair<std::string, Property>> GetEntityProperties();

    ordered_json SerializeObject() override;
    void DeSerializeObject(json jsonObject) override;

private:
    uint32_t m_Id;
    std::vector<uint32_t> m_SceneObjectIds;
    std::vector<uint32_t> m_SceneLightIds;
    uint32_t m_CameraId;
    uint32_t m_SkyboxId;
    SceneSettings m_SceneSettings;
    bool m_HasDirectionalLight;
    bool m_HasSkybox;
};