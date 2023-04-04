#pragma once
#include "Base.h"
#include "Entity/Entities/SceneObject.h"

class Scene
{
public:
    Scene();
    ~Scene();

    void AddSceneObject(Ref<SceneObject> object);

    const std::vector<Ref<SceneObject>>& GetSceneObjects() const { return m_SceneObjects; }
    int32_t* const GetSelectedObject() { return &m_SelectedObject; }
    
private:
    std::vector<Ref<SceneObject>> m_SceneObjects;
    int32_t m_SelectedObject;
};