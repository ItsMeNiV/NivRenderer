#pragma once
#include "Base.h"
#include "Entity/Entities/SceneObject.h"

class Scene
{
public:
    Scene();
    ~Scene();

    uint32_t AddSceneObject(int32_t parentObjectId = -1);
    void RemoveSceneObject(uint32_t sceneObjectId);

    const uint32_t GetId() const { return m_Id; }
    const std::vector<uint32_t>& GetSceneObjectIds() const { return m_SceneObjectIds; }
    
private:
    uint32_t m_Id;
    std::vector<uint32_t> m_SceneObjectIds;
};