#include "Entity/Components/MaterialComponent.h"

MaterialComponent::MaterialComponent(const uint32_t id) :
    Component(id, "MaterialComponent"), m_DiffusePath("default"), m_FlipDiffuseTexture(false), m_FlipNormalTexture(false),
    m_FlipMetallicTexture(false), m_FlipRoughnessTexture(false), m_FlipAOTexture(false), m_FlipEmissiveTexture(false)
{
    reloadDiffuseTexture();
}

std::vector<std::pair<std::string, ComponentProperty>> MaterialComponent::GetComponentProperties()
{
    std::vector<std::pair<std::string, ComponentProperty>> returnVector;

    returnVector.push_back({"Diffuse Path", {NivRenderer::PropertyType::PATH, &m_DiffusePath, [this]() { reloadDiffuseTexture(); }}});
    returnVector.push_back({"Flip Diffuse Texture", {NivRenderer::PropertyType::BOOL, &m_FlipDiffuseTexture, []() {return; }}});
    returnVector.push_back({"Normal Path", {NivRenderer::PropertyType::PATH, &m_NormalPath, [this]() { reloadNormalTexture(); }}});
    returnVector.push_back({"Flip Normal Texture", {NivRenderer::PropertyType::BOOL, &m_FlipNormalTexture, []() { return; }}});
    returnVector.push_back({"Metallic Path", {NivRenderer::PropertyType::PATH, &m_MetallicPath,[this]() { reloadMetallicTexture(); }}});
    returnVector.push_back({"Flip Metallic Texture", {NivRenderer::PropertyType::BOOL, &m_FlipMetallicTexture, []() { return; }}});
    returnVector.push_back({"Roughness Path", {NivRenderer::PropertyType::PATH, &m_RoughnessPath, [this]() { reloadRoughnessTexture(); }}});
    returnVector.push_back({"Flip Roughness Texture", {NivRenderer::PropertyType::BOOL, &m_FlipRoughnessTexture, []() { return; }}});
    returnVector.push_back({"AO Path", {NivRenderer::PropertyType::PATH, &m_AOPath, [this]() { reloadAOTexture(); }}});
    returnVector.push_back({"Flip AO Texture", {NivRenderer::PropertyType::BOOL, &m_FlipAOTexture, []() { return; }}});
    returnVector.push_back({"Emissive Path", {NivRenderer::PropertyType::PATH, &m_EmissivePath, [this]() { reloadEmissiveTexture(); }}});
    returnVector.push_back({"Flip Emissive Texture", {NivRenderer::PropertyType::BOOL, &m_FlipEmissiveTexture, []() { return; }}});

    return returnVector;
}

void MaterialComponent::reloadDiffuseTexture()
{
    if (m_DiffusePath.empty())
        return;

    m_DiffuseTextureAsset = AssetManager::GetInstance().LoadTexture(m_DiffusePath, m_FlipDiffuseTexture);
}

void MaterialComponent::reloadNormalTexture()
{
    if (m_NormalPath.empty())
        return;

    m_NormalTextureAsset = AssetManager::GetInstance().LoadTexture(m_NormalPath, m_FlipNormalTexture);
}

void MaterialComponent::reloadMetallicTexture()
{
    if (m_MetallicPath.empty())
        return;

    m_MetallicTextureAsset = AssetManager::GetInstance().LoadTexture(m_MetallicPath, m_FlipMetallicTexture);
}

void MaterialComponent::reloadRoughnessTexture()
{
    if (m_RoughnessPath.empty())
        return;

    m_RoughnessTextureAsset = AssetManager::GetInstance().LoadTexture(m_RoughnessPath, m_FlipRoughnessTexture);
}

void MaterialComponent::reloadAOTexture()
{
    if (m_AOPath.empty())
        return;

    m_AOTextureAsset = AssetManager::GetInstance().LoadTexture(m_AOPath, m_FlipAOTexture);
}

void MaterialComponent::reloadEmissiveTexture()
{
    if (m_EmissivePath.empty())
        return;

    m_EmissiveTextureAsset = AssetManager::GetInstance().LoadTexture(m_EmissivePath, m_FlipEmissiveTexture);
}
