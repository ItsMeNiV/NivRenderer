#include "Entity/Components/MaterialComponent.h"

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

ordered_json MaterialComponent::SerializeObject()
{
    ordered_json component = {
        {"Id", GetId()},
        {"Name", GetName()},
        {"MaterialAssetId", m_MaterialAsset->GetId()},
    };

    return component;
}
