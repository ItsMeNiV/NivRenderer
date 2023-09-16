#include "MaterialAsset.h"
#include "Entity/Assets/AssetManager.h"

MaterialAsset::MaterialAsset(uint32_t id, const std::string& name) :
    Asset(id), m_Name(name), m_DirtyFlag(true),
    m_DiffusePath("default"), m_DiffuseTextureAsset(nullptr), m_FlipDiffuseTexture(false),
    m_NormalTextureAsset(nullptr), m_FlipNormalTexture(false),
    m_MetallicTextureAsset(nullptr),
    m_FlipMetallicTexture(false), m_RoughnessTextureAsset(nullptr), m_FlipRoughnessTexture(false),
    m_AOTextureAsset(nullptr), m_FlipAOTexture(false),
    m_EmissiveTextureAsset(nullptr), m_FlipEmissiveTexture(false)
{
}

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

std::vector<std::pair<std::string, Property>> MaterialAsset::GetAssetProperties()
{
    std::vector<std::pair<std::string, Property>> returnVector;

    returnVector.push_back({"Name", {PropertyType::STRING, &m_Name, [](){}}});

    returnVector.push_back({"Diffuse", {PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"Diffuse Path", {PropertyType::PATH, &m_DiffusePath, [this]() { reloadDiffuseTexture(); }}});
    returnVector.push_back({"Flip Diffuse Texture", {PropertyType::BOOL, &m_FlipDiffuseTexture, []() {}}});

    returnVector.push_back({"Normal", {PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"Normal Path", {PropertyType::PATH, &m_NormalPath, [this]() { reloadNormalTexture(); }}});
    returnVector.push_back({"Flip Normal Texture", {PropertyType::BOOL, &m_FlipNormalTexture, []() {}}});

    returnVector.push_back({"Metallic", {PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"Metallic Path", {PropertyType::PATH, &m_MetallicPath, [this]() { reloadMetallicTexture(); }}});
    returnVector.push_back({"Flip Metallic Texture", {PropertyType::BOOL, &m_FlipMetallicTexture, []() {}}});

    returnVector.push_back({"Roughness", {PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"Roughness Path", {PropertyType::PATH, &m_RoughnessPath, [this]() { reloadRoughnessTexture(); }}});
    returnVector.push_back({"Flip Roughness Texture", {PropertyType::BOOL, &m_FlipRoughnessTexture, []() {}}});

    returnVector.push_back({"AO", {PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"AO Path", {PropertyType::PATH, &m_AOPath, [this]() { reloadAOTexture(); }}});
    returnVector.push_back({"Flip AO Texture", {PropertyType::BOOL, &m_FlipAOTexture, []() {}}});

    returnVector.push_back({"Emissive", {PropertyType::SEPARATORTEXT, nullptr, [this]() {}}});
    returnVector.push_back({"Emissive Path", {PropertyType::PATH, &m_EmissivePath, [this]() { reloadEmissiveTexture(); }}});
    returnVector.push_back({"Flip Emissive Texture", {PropertyType::BOOL, &m_FlipEmissiveTexture, []() {}}});

    return returnVector;
}

ordered_json MaterialAsset::SerializeObject()
{
    ordered_json material = {
        {"Id", GetId()},
        {"Name", m_Name},
    };
    if (m_DiffuseTextureAsset)
    {
        material["Diffuse"] = {
            {"Path", m_DiffusePath},
            {"TextureAssetId", m_DiffuseTextureAsset->GetId()},
            {"Flip", m_FlipDiffuseTexture}
        };
    }
    if (m_NormalTextureAsset)
    {
        material["Normal"] = {
            {"Path", m_NormalPath},
            {"TextureAssetId", m_NormalTextureAsset->GetId()},
            {"Flip", m_FlipNormalTexture}
        };
    }
    if (m_MetallicTextureAsset)
    {
        material["Metallic"] = {
            {"Path", m_MetallicPath},
            {"TextureAssetId", m_MetallicTextureAsset->GetId()},
            {"Flip", m_FlipMetallicTexture}
        };
    }
    if (m_RoughnessTextureAsset)
    {
        material["Roughness"] = {
            {"Path", m_RoughnessPath},
            {"TextureAssetId", m_RoughnessTextureAsset->GetId()},
            {"Flip", m_FlipRoughnessTexture}
        };
    }
    if (m_AOTextureAsset)
    {
        material["AO"] = {
            {"Path", m_AOPath},
            {"TextureAssetId", m_AOTextureAsset->GetId()},
            {"Flip", m_FlipAOTexture}
        };
    }
    if (m_EmissiveTextureAsset)
    {
        material["Emissive"] = {
            {"Path", m_EmissivePath},
            {"TextureAssetId", m_EmissiveTextureAsset->GetId()},
            {"Flip", m_FlipEmissiveTexture}
        };
    }

    return material;
}

void MaterialAsset::DeSerializeObject(json jsonObject)
{
    if (jsonObject.contains("Diffuse"))
    {
        json diffuse = jsonObject["Diffuse"];
        m_DiffusePath = diffuse["Path"];
        m_FlipDiffuseTexture = diffuse["Flip"];
        m_DiffuseTextureAsset = AssetManager::GetInstance().LoadTexture(m_DiffusePath, m_FlipDiffuseTexture);
    }
}
