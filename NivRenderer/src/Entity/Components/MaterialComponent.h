#pragma once
#include "Base.h"
#include "Entity/Component.h"

class MaterialComponent : public Component
{
public:
    MaterialComponent() : MaterialComponent(std::string(""), std::string("")) {}
    MaterialComponent(std::string &&diffusePath) : MaterialComponent(std::move(diffusePath), std::string("")) {}
    MaterialComponent(std::string &&diffusePath, std::string &&specularPath) :
        Component("MaterialComponent"), m_DiffusePath(std::move(diffusePath)), m_SpecularPath(std::move(specularPath)) {}

    ~MaterialComponent() = default;

    virtual std::unordered_map<std::string, ComponentProperty> GetComponentProperties()
    {
        std::unordered_map<std::string, ComponentProperty> returnMap;

        returnMap["Diffuse Path"] = {PropertyType::PATH, &m_DiffusePath, [this]() { reloadTexture(); }};
        returnMap["Specular Path"] = {PropertyType::PATH, &m_SpecularPath, [this]() { reloadTexture(); }};

        return returnMap;
    }

    const std::string& GetDiffusePath() const { return m_DiffusePath; }
    const std::string& GetSpecularPath() const { return m_SpecularPath; }

private:
    std::string m_DiffusePath;
    std::string m_SpecularPath;

    void reloadTexture()
    {
        std::cout << "Reloading Texture" << std::endl;
    }
};