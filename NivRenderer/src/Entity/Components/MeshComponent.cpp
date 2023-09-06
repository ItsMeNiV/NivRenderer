#include "Entity/Components/MeshComponent.h"

MeshComponent::MeshComponent(const uint32_t id)
    : Component(id, "MeshComponent"), m_Path("default")
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

std::vector<std::pair<std::string, Property>> MeshComponent::GetComponentProperties()
{
    std::vector<std::pair<std::string, Property>> returnVector;

    returnVector.push_back({"Path", {PropertyType::PATH, &m_Path, [this]() { reloadMesh(); }}});

    return returnVector;
}

void MeshComponent::reloadMesh()
{
    m_MeshAsset = AssetManager::GetInstance().LoadMesh(m_Path);
}
