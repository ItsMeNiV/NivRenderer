#include "Entity/Components/MeshComponent.h"

MeshComponent::MeshComponent()
    : Component("MeshComponent"), m_Path("default")
{
    reloadMesh();
}

const Ref<MeshAsset>& MeshComponent::GetMeshAsset() const
{
    return m_MeshAsset;
}

std::unordered_map<std::string, ComponentProperty> MeshComponent::GetComponentProperties()
{
    std::unordered_map<std::string, ComponentProperty> returnMap;

    returnMap["Path"] = {NivRenderer::PropertyType::PATH, &m_Path, [this](){
        reloadMesh();
    }};

    return returnMap;
}

void MeshComponent::reloadMesh()
{
    m_MeshAsset = AssetManager::GetInstance().LoadMesh(m_Path);
}
