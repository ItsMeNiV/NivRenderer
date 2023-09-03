#include "MaterialAsset.h"
#include "Entity/Assets/AssetManager.h"

MaterialAsset::MaterialAsset(uint32_t id, const char* name) :
    Asset(id), m_Name(name), m_DiffusePath("default"),
    m_FlipDiffuseTexture(false), m_FlipNormalTexture(false), m_FlipMetallicTexture(false),
    m_FlipRoughnessTexture(false), m_FlipAOTexture(false), m_FlipEmissiveTexture(false)
{
    reloadDiffuseTexture();
}

/*
std::vector<std::pair<std::string, NivRenderer::Property>> MaterialAsset::GetEntityProperties()
{
    std::vector<std::pair<std::string, NivRenderer::Property>> returnVector;

    returnVector.push_back({"Diffuse", {NivRenderer::PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"Diffuse Path", {NivRenderer::PropertyType::PATH, &m_DiffusePath, [this]() { reloadDiffuseTexture(); }}});
    returnVector.push_back({"Flip Diffuse Texture", {NivRenderer::PropertyType::BOOL, &m_FlipDiffuseTexture, []() { return; }}});

    returnVector.push_back({"Normal", {NivRenderer::PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"Normal Path", {NivRenderer::PropertyType::PATH, &m_NormalPath, [this]() { reloadNormalTexture(); }}});
    returnVector.push_back({"Flip Normal Texture", {NivRenderer::PropertyType::BOOL, &m_FlipNormalTexture, []() { return; }}});

    returnVector.push_back({"Metallic", {NivRenderer::PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"Metallic Path", {NivRenderer::PropertyType::PATH, &m_MetallicPath, [this]() { reloadMetallicTexture(); }}});
    returnVector.push_back({"Flip Metallic Texture", {NivRenderer::PropertyType::BOOL, &m_FlipMetallicTexture, []() { return; }}});

    returnVector.push_back({"Roughness", {NivRenderer::PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"Roughness Path", {NivRenderer::PropertyType::PATH, &m_RoughnessPath, [this]() { reloadRoughnessTexture(); }}});
    returnVector.push_back({"Flip Roughness Texture", {NivRenderer::PropertyType::BOOL, &m_FlipRoughnessTexture, []() { return; }}});

    returnVector.push_back({"AO", {NivRenderer::PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"AO Path", {NivRenderer::PropertyType::PATH, &m_AOPath, [this]() { reloadAOTexture(); }}});
    returnVector.push_back({"Flip AO Texture", {NivRenderer::PropertyType::BOOL, &m_FlipAOTexture, []() { return; }}});

    returnVector.push_back({"Emissive", {NivRenderer::PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"Emissive Path", {NivRenderer::PropertyType::PATH, &m_EmissivePath, [this]() { reloadEmissiveTexture(); }}});
    returnVector.push_back({"Flip Emissive Texture", {NivRenderer::PropertyType::BOOL, &m_FlipEmissiveTexture, []() { return; }}});

    return returnVector;
}*/

void MaterialAsset::reloadDiffuseTexture()
{
    if (m_DiffusePath.empty())
        return;

    m_DiffuseTextureAsset = AssetManager::GetInstance().LoadTexture(m_DiffusePath, m_FlipDiffuseTexture);
}

void MaterialAsset::reloadNormalTexture()
{
    if (m_NormalPath.empty())
        return;

    m_NormalTextureAsset = AssetManager::GetInstance().LoadTexture(m_NormalPath, m_FlipNormalTexture);
}

void MaterialAsset::reloadMetallicTexture()
{
    if (m_MetallicPath.empty())
        return;

    m_MetallicTextureAsset = AssetManager::GetInstance().LoadTexture(m_MetallicPath, m_FlipMetallicTexture);
}

void MaterialAsset::reloadRoughnessTexture()
{
    if (m_RoughnessPath.empty())
        return;

    m_RoughnessTextureAsset = AssetManager::GetInstance().LoadTexture(m_RoughnessPath, m_FlipRoughnessTexture);
}

void MaterialAsset::reloadAOTexture()
{
    if (m_AOPath.empty())
        return;

    m_AOTextureAsset = AssetManager::GetInstance().LoadTexture(m_AOPath, m_FlipAOTexture);
}

void MaterialAsset::reloadEmissiveTexture()
{
    if (m_EmissivePath.empty())
        return;

    m_EmissiveTextureAsset = AssetManager::GetInstance().LoadTexture(m_EmissivePath, m_FlipEmissiveTexture);
}
