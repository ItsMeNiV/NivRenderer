#pragma once
#include "Base.h"
#include "Entity/Assets/MeshAsset.h"
#include "Entity/Assets/TextureAsset.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

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

    void loadDefaultMeshAndTextures();
    void processNode(const aiNode* node, const aiScene* scene, std::vector<Ref<SubMesh>>& subMeshes);
    Ref<SubMesh> processMesh(aiMesh* mesh, const aiScene* scene);
};
