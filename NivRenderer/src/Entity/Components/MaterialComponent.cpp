#include "Entity/Components/MaterialComponent.h"

#include "Entity/Assets/AssetManager.h"

MaterialComponent::MaterialComponent(const uint32_t id) :
    Component(id, "MaterialComponent"), m_MaterialAsset(nullptr)
{
    
}

std::vector<std::pair<std::string, Property>> MaterialComponent::GetComponentProperties()
{
    std::vector<std::pair<std::string, Property>> returnVector;

    returnVector.push_back({"Material", {PropertyType::MATERIALDROPDOWN, &m_MaterialAsset, []() {}}});

    return returnVector;
}

nlohmann::ordered_json MaterialComponent::SerializeObject()
{
    nlohmann::ordered_json component = {
        {"Id", GetId()},
        {"Type", "MaterialComponent"},
        {"Name", GetName()},
        {"MaterialAssetId", m_MaterialAsset->GetId()},
    };

    return component;
}

void MaterialComponent::DeSerializeObject(nlohmann::json jsonObject)
{
    m_MaterialAsset = AssetManager::GetInstance().GetMaterial(static_cast<uint32_t>(jsonObject["MaterialAssetId"]));
}
