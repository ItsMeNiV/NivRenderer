#pragma once
#include "Base.h"
#include "Entity/Assets/MeshAsset.h"
#include "Entity/Assets/TextureAsset.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

// Struct to associate a Modelpath with multiple Texture paths
// TODO: Maybe redesign this so the Model doesn't need to know how many and which kind of Texture paths are needed per Model
struct Model
{
    std::string meshPath;
    std::string diffuseTexturePath;
    std::string normalTexturePath;
    std::string metallicTexturePath;
    std::string roughnessTexturePath;
    std::string aoTexturePath;
    std::string emissiveTexturePath;
};

class AssetManager
{
public:
    AssetManager(AssetManager const &) = delete;
    void operator=(AssetManager const &) = delete;

    static AssetManager& GetInstance()
    {
        static AssetManager instance;
        return instance;
    }

    Ref<MeshAsset> LoadMesh(const std::string& path);
    Ref<TextureAsset> LoadTexture(std::string& path, bool flipVertical, bool loadOnlyOneChannel = false, int channelIndex = 0);
    Ref<Shader> LoadShader(const std::string& path, ShaderType shaderType);
    std::vector<std::string> LoadMeshAndTextures(std::string& path, Ref<MeshAsset>& mesh,
                                                 Ref<TextureAsset>& diffuseTexture, Ref<TextureAsset>& normalTexture,
                                                 Ref<TextureAsset>& metallicTexture, Ref<TextureAsset>& roughnessTexture, Ref<TextureAsset>& aoTexture,
                                                 Ref<TextureAsset>& emissiveTexture,
                                                 bool flipVerticalDiffuse, bool flipVerticalNormal,
                                                 bool flipVerticalMetallic,
                                                 bool flipVerticalRoughness, bool flipVerticalAO, bool flipVerticalEmissive);

private:
    AssetManager();

    Scope<Assimp::Importer> m_Importer;
    std::unordered_map<std::string, Ref<MeshAsset>> m_LoadedMeshAssets;
    std::unordered_map<std::string, Ref<TextureAsset>> m_LoadedTextureAssets;
    std::unordered_map<std::string, Ref<Shader>> m_LoadedShaders;
    std::unordered_map<std::string, Model> m_LoadedModels;

    void loadDefaultMeshAndTextures();
    void processNode(const aiNode* node, const aiScene* scene, std::vector<Ref<SubMesh>>& subMeshes);
    Ref<SubMesh> processMesh(aiMesh* mesh, const aiScene* scene);
};
