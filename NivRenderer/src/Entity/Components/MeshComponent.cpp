#include "Entity/Components/MeshComponent.h"

MeshComponent::MeshComponent(const uint32_t id)
    : Component(id, "MeshComponent"), m_Path("default")
{
    reloadMesh();
}

MeshAsset* MeshComponent::GetMeshAsset() const
{
    return m_MeshAsset;
}

std::string& MeshComponent::GetPath()
{
    return m_Path;
}

void MeshComponent::SetMeshAsset(MeshAsset* asset)
{
    m_MeshAsset = asset;
}

std::vector<std::pair<std::string, Property>> MeshComponent::GetComponentProperties()
{
    std::vector<std::pair<std::string, Property>> returnVector;

    returnVector.push_back({"Path", {PropertyType::PATH, &m_Path, [this]() { reloadMesh(); }}});

    return returnVector;
}

ordered_json MeshComponent::SerializeObject()
{
    ordered_json component = {
        {"Id", GetId()},
        {"Type", "MeshComponent"},
        {"Name", GetName()},
        {"Path", m_Path},
        {"MeshAssetId", m_MeshAsset->GetId()},
    };

    return component;
}

void MeshComponent::DeSerializeObject(json jsonObject)
{
    m_Path = jsonObject["Path"];
    m_MeshAsset = AssetManager::GetInstance().GetMesh(jsonObject["MeshAssetId"]);
}

void MeshComponent::reloadMesh()
{
    m_MeshAsset = AssetManager::GetInstance().LoadMesh(m_Path);
}
