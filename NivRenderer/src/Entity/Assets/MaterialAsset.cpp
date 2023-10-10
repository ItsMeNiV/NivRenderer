#include "MaterialAsset.h"
#include "Entity/Assets/AssetManager.h"

MaterialAsset::MaterialAsset(uint32_t id, const std::string& name) :
    m_Id(id), m_Name(name), m_DirtyFlag(true),
    m_DiffusePath("default"), m_DiffuseTextureAsset(nullptr), m_FlipDiffuseTexture(false),
    m_NormalTextureAsset(nullptr), m_FlipNormalTexture(false),
    m_MetallicTextureAsset(nullptr),
    m_FlipMetallicTexture(false), m_RoughnessTextureAsset(nullptr), m_FlipRoughnessTexture(false),
    m_AOTextureAsset(nullptr), m_FlipAOTexture(false),
    m_EmissiveTextureAsset(nullptr), m_FlipEmissiveTexture(false)
{
}

void MaterialAsset::ReloadDiffuseTexture()
{
    if (m_DiffusePath.empty())
        return;

    m_DiffuseTextureAsset = AssetManager::GetInstance().LoadTexture(m_DiffusePath, m_FlipDiffuseTexture);
    m_DirtyFlag = true;
}

void MaterialAsset::ReloadNormalTexture()
{
    if (m_NormalPath.empty())
        return;

    m_NormalTextureAsset = AssetManager::GetInstance().LoadTexture(m_NormalPath, m_FlipNormalTexture);
    m_DirtyFlag = true;
}

void MaterialAsset::ReloadMetallicTexture()
{
    if (m_MetallicPath.empty())
        return;

    m_MetallicTextureAsset = AssetManager::GetInstance().LoadTexture(m_MetallicPath, m_FlipMetallicTexture);
    m_DirtyFlag = true;
}

void MaterialAsset::ReloadRoughnessTexture()
{
    if (m_RoughnessPath.empty())
        return;

    m_RoughnessTextureAsset = AssetManager::GetInstance().LoadTexture(m_RoughnessPath, m_FlipRoughnessTexture);
    m_DirtyFlag = true;
}

void MaterialAsset::ReloadAOTexture()
{
    if (m_AOPath.empty())
        return;

    m_AOTextureAsset = AssetManager::GetInstance().LoadTexture(m_AOPath, m_FlipAOTexture);
    m_DirtyFlag = true;
}

void MaterialAsset::ReloadEmissiveTexture()
{
    if (m_EmissivePath.empty())
        return;

    m_EmissiveTextureAsset = AssetManager::GetInstance().LoadTexture(m_EmissivePath, m_FlipEmissiveTexture);
    m_DirtyFlag = true;
}

nlohmann::ordered_json MaterialAsset::SerializeObject()
{
    nlohmann::ordered_json material = {
        {"Id", m_Id},
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

void MaterialAsset::DeSerializeObject(nlohmann::json jsonObject)
{
    if (jsonObject.contains("Diffuse"))
    {
        nlohmann::json diffuse = jsonObject["Diffuse"];
        m_DiffusePath = diffuse["Path"];
        m_FlipDiffuseTexture = diffuse["Flip"];
        m_DiffuseTextureAsset = AssetManager::GetInstance().GetTexture(diffuse["TextureAssetId"]);
    }
    if (jsonObject.contains("Normal"))
    {
        nlohmann::json normal = jsonObject["Normal"];
        m_NormalPath = normal["Path"];
        m_FlipNormalTexture = normal["Flip"];
        m_NormalTextureAsset = AssetManager::GetInstance().GetTexture(normal["TextureAssetId"]);
    }
    if (jsonObject.contains("Metallic"))
    {
        nlohmann::json metallic = jsonObject["Metallic"];
        m_MetallicPath = metallic["Path"];
        m_FlipMetallicTexture = metallic["Flip"];
        m_MetallicTextureAsset = AssetManager::GetInstance().GetTexture(metallic["TextureAssetId"]);
    }
    if (jsonObject.contains("Roughness"))
    {
        nlohmann::json roughness = jsonObject["Roughness"];
        m_RoughnessPath = roughness["Path"];
        m_FlipRoughnessTexture = roughness["Flip"];
        m_RoughnessTextureAsset = AssetManager::GetInstance().GetTexture(roughness["TextureAssetId"]);
    }
    if (jsonObject.contains("AO"))
    {
        nlohmann::json ao = jsonObject["AO"];
        m_AOPath = ao["Path"];
        m_FlipAOTexture = ao["Flip"];
        m_AOTextureAsset = AssetManager::GetInstance().GetTexture(ao["TextureAssetId"]);
    }
    if (jsonObject.contains("Emissive"))
    {
        nlohmann::json emissive = jsonObject["Emissive"];
        m_EmissivePath = emissive["Path"];
        m_FlipEmissiveTexture = emissive["Flip"];
        m_EmissiveTextureAsset = AssetManager::GetInstance().GetTexture(emissive["TextureAssetId"]);
    }
}
