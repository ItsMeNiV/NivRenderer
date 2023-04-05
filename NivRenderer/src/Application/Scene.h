#pragma once
#include "Base.h"
#include "Entity/Entities/SceneObject.h"

class Scene
{
public:
    Scene();
    ~Scene();

    uint32_t AddSceneObject(int32_t parentObjectId = -1);

    const std::vector<uint32_t>& GetSceneObjectIds() const { return m_SceneObjectIds; }
    
private:
    std::vector<uint32_t> m_SceneObjectIds;
};