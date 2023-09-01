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
    auto& normalTexture = material->GetNormalTextureAsset();
    auto& metallicTexture = material->GetMetallicTextureAsset();
    auto& roughnessTexture = material->GetRoughnessTextureAsset();
    auto& aoTexture = material->GetAOTextureAsset();
    auto& emissiveTexture = material->GetEmissiveTextureAsset();

    std::vector<std::string> meshAndTexturePaths = AssetManager::GetInstance().LoadMeshAndTextures(
        m_ModelPath, mesh->GetMeshAsset(), diffuseTexture, normalTexture, metallicTexture, roughnessTexture, aoTexture, emissiveTexture,
        material->GetFlipDiffuseTexture(), material->GetFlipNormalTexture(), material->GetFlipMetallicTexture(),
        material->GetFlipRoughnessTexture(), material->GetFlipAOTexture(), material->GetFlipEmissiveTexture());

    //Assumes that we always get back 7 paths
    if (meshAndTexturePaths.size() == 7)
    {
        mesh->GetPath() = meshAndTexturePaths[0].empty() ? std::string("default") : meshAndTexturePaths[0];
        material->GetDiffusePath() = meshAndTexturePaths[1].empty() ? std::string("default") : meshAndTexturePaths[1];
        material->GetNormalPath() = meshAndTexturePaths[2];
        material->GetMetallicPath() = meshAndTexturePaths[3];
        material->GetRoughnessPath() = meshAndTexturePaths[4];
        material->GetAOPath() = meshAndTexturePaths[5];
        material->GetEmissivePath() = meshAndTexturePaths[6];
    }
}

std::vector<std::pair<std::string, NivRenderer::Property>> SceneObject::GetEntityProperties()
{
    std::vector<std::pair<std::string, NivRenderer::Property>> returnVector;

    return returnVector;
}
