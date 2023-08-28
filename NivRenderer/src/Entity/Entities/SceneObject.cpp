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
    Ref<MeshComponent> mesh = ECSRegistry::GetInstance().GetComponent<MeshComponent>(GetId());
    Ref<MaterialComponent> material = ECSRegistry::GetInstance().GetComponent<MaterialComponent>(GetId());
    auto& diffuseTexture = material->GetDiffuseTextureAsset();
    auto& specularTexture = material->GetSpecularTextureAsset();
    auto& normalTexture = material->GetNormalTextureAsset();

    std::vector<std::string> meshAndTexturePaths = AssetManager::GetInstance().LoadMeshAndTextures(
        m_ModelPath, mesh->GetMeshAsset(), diffuseTexture, specularTexture, normalTexture,
        material->GetFlipDiffuseTexture(), material->GetFlipSpecularTexture(), material->GetFlipNormalTexture());

    //Assumes that we always get back 4 paths
    if (meshAndTexturePaths.size() == 4)
    {
        mesh->GetPath() = meshAndTexturePaths[0].empty() ? std::string("default") : meshAndTexturePaths[0];
        material->GetDiffusePath() = meshAndTexturePaths[1].empty() ? std::string("default") : meshAndTexturePaths[1];
        material->GetSpecularPath() = meshAndTexturePaths[2];
        material->GetNormalPath() = meshAndTexturePaths[3];   
    }
}
