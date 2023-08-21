#include "Entity/Components/MaterialComponent.h"

MaterialComponent::MaterialComponent(): MaterialComponent(std::string("default"), std::string(""))
{}

MaterialComponent::MaterialComponent(std::string&& diffusePath): MaterialComponent(std::move(diffusePath), std::string(""))
{}

MaterialComponent::MaterialComponent(std::string&& diffusePath, std::string&& specularPath): MaterialComponent(std::move(diffusePath), std::move(specularPath), std::string(""))
{}

MaterialComponent::MaterialComponent(std::string&& diffusePath, std::string&& specularPath, std::string&& normalPath) :
    Component("MaterialComponent"), m_DiffusePath(std::move(diffusePath)), m_SpecularPath(std::move(specularPath)), m_NormalPath(std::move(normalPath)),
    m_FlipDiffuseTexture(false), m_FlipSpecularTexture(false), m_FlipNormalTexture(false)
{
    reloadDiffuseTexture();
    reloadSpecularTexture();
    reloadNormalTexture();
}

std::unordered_map<std::string, ComponentProperty> MaterialComponent::GetComponentProperties()
{
    std::unordered_map<std::string, ComponentProperty> returnMap;

    returnMap["Diffuse Path"] = {NivRenderer::PropertyType::PATH, &m_DiffusePath, [this]() { reloadDiffuseTexture(); }};
    returnMap["Flip Diffuse Texture"] = {NivRenderer::PropertyType::BOOL, &m_FlipDiffuseTexture, []() { return; }};
    returnMap["Specular Path"] = {NivRenderer::PropertyType::PATH, &m_SpecularPath, [this]() { reloadSpecularTexture(); }};
    returnMap["Flip Specular Texture"] = {NivRenderer::PropertyType::BOOL, &m_FlipSpecularTexture, []() { return; }};
    returnMap["Normal Path"] = {NivRenderer::PropertyType::PATH, &m_NormalPath, [this]() { reloadNormalTexture(); }};
    returnMap["Flip Normal Texture"] = {NivRenderer::PropertyType::BOOL, &m_FlipNormalTexture, []() { return; }};

    return returnMap;
}

void MaterialComponent::reloadDiffuseTexture()
{
    if (m_DiffusePath.empty())
        return;

    m_DiffuseTextureAsset = AssetManager::GetInstance().LoadTexture(m_DiffusePath, m_FlipDiffuseTexture);
}

void MaterialComponent::reloadSpecularTexture()
{
    if (m_SpecularPath.empty())
        return;

    m_SpecularTextureAsset = AssetManager::GetInstance().LoadTexture(m_SpecularPath, m_FlipSpecularTexture);
}

void MaterialComponent::reloadNormalTexture()
{
    if (m_NormalPath.empty())
        return;

    m_NormalTextureAsset = AssetManager::GetInstance().LoadTexture(m_NormalPath, m_FlipNormalTexture);
}
