#include "Scene.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/TransformComponent.h"

Scene::Scene()
    : m_SelectedObject(-1)
{
}

Scene::~Scene()
{
}

void Scene::AddSceneObject(Ref<SceneObject> object)
{
    m_SceneObjects.push_back(object);
    Ref<TransformComponent> c1 = CreateRef<TransformComponent>();
    ECSRegistry::GetInstance().AddEntity(object);
    ECSRegistry::GetInstance().AddComponent(object->GetId(), c1);
}
