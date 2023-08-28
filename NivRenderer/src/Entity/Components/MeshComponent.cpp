#include "Entity/Components/MeshComponent.h"

MeshComponent::MeshComponent()
    : Component("MeshComponent"), m_Path("default")
{
    reloadMesh();
}

Ref<MeshAsset>& MeshComponent::GetMeshAsset()
{
    return m_MeshAsset;
}

std::string& MeshComponent::GetPath()
{
    return m_Path;
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
