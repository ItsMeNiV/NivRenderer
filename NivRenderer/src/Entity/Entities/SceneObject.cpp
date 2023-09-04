#include "SceneObject.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Components/MaterialComponent.h"
#include "Entity/Components/MeshComponent.h"

SceneObject::SceneObject(uint32_t id)
	: Entity(id, std::string("SceneObject (") + std::to_string(id) + std::string(")")), m_ModelPath("")
{
}

void SceneObject::LoadMeshAndMaterial()
{
    // Assumes that all SceneObjects have a mesh and material
    const Ref<MeshComponent> mesh = ECSRegistry::GetInstance().GetComponent<MeshComponent>(GetId());
    const Ref<MaterialComponent> materialComponent = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(GetId());
    const Model* model = AssetManager::GetInstance().LoadMeshAndTextures(m_ModelPath, mesh->GetMeshAsset());
    mesh->GetPath() = model->meshPath;
    materialComponent->GetMaterialAsset() = model->material;
}

std::vector<std::pair<std::string, NivRenderer::Property>> SceneObject::GetEntityProperties()
{
    std::vector<std::pair<std::string, NivRenderer::Property>> returnVector;

    returnVector.push_back({"Model Name", {NivRenderer::PropertyType::STRING, &m_EntityName, [this](){}}});
    returnVector.push_back({"Model Path", {NivRenderer::PropertyType::PATH, &m_ModelPath, [this]() { LoadMeshAndMaterial(); }}});

    return returnVector;
}
