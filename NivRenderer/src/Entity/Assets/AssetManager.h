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
    Ref<TextureAsset> LoadTexture(const std::string& path);

private:
    AssetManager();

    std::unordered_map<std::string, Ref<MeshAsset>> m_LoadedMeshAssets;
    std::unordered_map<std::string, Ref<TextureAsset>> m_LoadedTextureAssets;

    void loadDefaultMeshAndTextures();
    void processNode(const aiNode* node, const aiScene* scene, std::vector<Ref<SubMesh>>& subMeshes);
    Ref<SubMesh> processMesh(aiMesh* mesh, const aiScene* scene);
};
