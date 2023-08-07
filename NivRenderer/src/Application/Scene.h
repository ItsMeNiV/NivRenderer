#pragma once
#include "Base.h"
#include "Entity/Entities/SceneObject.h"
#include "Entity/Entities/LightObject.h"
#include "Entity/Entities/CameraObject.h"

class Scene
{
public:
    Scene();
    ~Scene();

    uint32_t AddSceneObject(int32_t parentObjectId = -1);
    void RemoveSceneObject(uint32_t sceneObjectId);
    uint32_t AddSceneDirectionalLight();
    uint32_t AddPointLight();
    void RemoveSceneLight(uint32_t sceneLightId);

    uint32_t AddCamera(const Ref<Camera> cameraPtr);

    const uint32_t GetId() const { return m_Id; }
    const std::vector<uint32_t>& GetSceneObjectIds() const { return m_SceneObjectIds; }
    const std::vector<uint32_t>& GetSceneLightIds() const { return m_SceneLightIds; }
    const uint32_t GetCameraId() const { return m_CameraId; }
    
private:
    uint32_t m_Id;
    std::vector<uint32_t> m_SceneObjectIds;
    std::vector<uint32_t> m_SceneLightIds;
    uint32_t m_CameraId;
};