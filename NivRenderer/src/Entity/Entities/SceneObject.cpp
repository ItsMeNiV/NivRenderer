#include "SceneObject.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/MaterialComponent.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/TransformComponent.h"

SceneObject::SceneObject(uint32_t id)
	: Entity(id, std::string("SceneObject (") + std::to_string(id) + std::string(")")), m_ModelPath("")
{
}

void SceneObject::LoadMeshAndMaterial()
{
    Ref<MeshComponent> meshComponent = ECSRegistry::GetInstance().GetComponent<MeshComponent>(GetId());
    Ref<MaterialComponent> materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(GetId());
    Ref<TransformComponent> transformComponent = ECSRegistry::GetInstance().GetComponent<TransformComponent>(GetId());
    if (!meshComponent)
    {
        ECSRegistry::GetInstance().AddComponent<MeshComponent>(GetId());
        meshComponent = ECSRegistry::GetInstance().GetComponent<MeshComponent>(GetId());
    }
    if (!materialComponent)
    {
        ECSRegistry::GetInstance().AddComponent<MaterialComponent>(GetId());
        materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(GetId());
    }
    if (!transformComponent)
    {
        ECSRegistry::GetInstance().AddComponent<TransformComponent>(GetId());
    }

    const Model* model = AssetManager::GetInstance().LoadMeshAndTextures(m_ModelPath, meshComponent->GetMeshAsset());
    meshComponent->GetPath() = model->meshPath;
    materialComponent->GetMaterialAsset() = model->material;
}

std::vector<std::pair<std::string, Property>> SceneObject::GetEntityProperties()
{
    std::vector<std::pair<std::string, Property>> returnVector;

    returnVector.push_back({"Model Name", {PropertyType::STRING, &m_EntityName, [this](){}}});
    returnVector.push_back({"Model Path", {PropertyType::PATH, &m_ModelPath, [this]() { LoadMeshAndMaterial(); }}});

    return returnVector;
}
