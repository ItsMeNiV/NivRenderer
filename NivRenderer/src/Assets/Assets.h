#pragma once
#include "Base.h"

struct TextureAsset
{
    uint32_t id = UINT32_MAX;
    std::string path = "";
    bool flipVertical = false;
    bool loadOnlyOneChannel = false;
    int32_t width = -1;
    int32_t height = -1;
    int32_t nrComponents = -1;
    int32_t channelIndex = -1;
    unsigned char* textureData = nullptr;

    TextureAsset() = default;
    TextureAsset(const TextureAsset&) = default;
    TextureAsset(uint32_t Id, const std::string& Path, bool FlipVertical, bool LoadOnlyOneChannel, int32_t ChannelIndex) :
        id(Id), path(Path), flipVertical(FlipVertical), loadOnlyOneChannel(LoadOnlyOneChannel), channelIndex(ChannelIndex) {}
    TextureAsset(uint32_t Id, const std::string& Path, bool FlipVertical, bool LoadOnlyOneChannel,
                 int32_t ChannelIndex, int32_t Width, int32_t Height, int32_t NrComponents)
    : id(Id), path(Path), flipVertical(FlipVertical), loadOnlyOneChannel(LoadOnlyOneChannel),
    width(Width), height(Height), nrComponents(NrComponents), channelIndex(ChannelIndex) {}
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
    MeshAsset(uint32_t Id, const std::string& Path, std::vector<MeshVertex>& Vertices, std::vector<uint32_t>& Indices) :
        id(Id), path(Path), vertices(Vertices), indices(Indices)
    {
    }
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