#pragma once
#include "Base.h"

struct TextureAsset
{

};

struct MeshVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};
struct MeshAsset
{
    uint32_t id = UINT32_MAX;
    std::string path = "";
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

    MeshAsset() = default;
    MeshAsset(const MeshAsset&) = default;
    MeshAsset(uint32_t Id, const std::string& Path) : id(Id), path(Path) {}
};

struct ShaderAsset
{
    uint32_t id = UINT32_MAX;
    Shader* shaderPtr = nullptr;

    ShaderAsset() = default;
    ShaderAsset(const ShaderAsset&) = default;
    ShaderAsset(uint32_t Id, Shader* ShaderPtr) : id(Id), shaderPtr(ShaderPtr) {}
};

struct MaterialAsset
{
    uint32_t id = UINT32_MAX;
    std::string name = "";

    // Diffuse/Albedo
    std::string diffusePath = "";
    TextureAsset* diffuseTextureAsset = nullptr;
    bool flipDiffuseTexture = false;
    // Normal
    std::string normalPath = "";
    TextureAsset* normalTextureAsset = nullptr;
    bool flipNormalTexture = false;
    // Metallic
    std::string metallicPath = "";
    TextureAsset* metallicTextureAsset = nullptr;
    bool flipMetallicTexture = false;
    // Roughness
    std::string roughnessPath = "";
    TextureAsset* roughnessTextureAsset = nullptr;
    bool flipRoughnessTexture = false;
    // AO
    std::string aoPath = "";
    TextureAsset* aoTextureAsset = nullptr;
    bool flipAOTexture = false;
    // Emissive
    std::string emissivePath = "";
    TextureAsset* emissiveTextureAsset = nullptr;
    bool flipEmissiveTexture = false;

    MaterialAsset() = default;
    MaterialAsset(const MaterialAsset&) = default;
    MaterialAsset(uint32_t Id, const std::string& Name) : id(Id), name(Name) {}
};