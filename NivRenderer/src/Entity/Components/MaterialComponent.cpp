#include "Entity/Components/MaterialComponent.h"

MaterialComponent::MaterialComponent(const uint32_t id) :
    Component(id, "MaterialComponent"), m_MaterialAsset(nullptr)
{
    
}

std::vector<std::pair<std::string, NivRenderer::Property>> MaterialComponent::GetComponentProperties()
{
    std::vector<std::pair<std::string, NivRenderer::Property>> returnVector;

    returnVector.push_back({"Material", {NivRenderer::PropertyType::MATERIALDROPDOWN, &m_MaterialAsset, []() {}}});

    return returnVector;
}
