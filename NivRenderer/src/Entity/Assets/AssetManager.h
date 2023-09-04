#pragma once
#include "Base.h"
#include "MaterialAsset.h"
#include "Entity/Assets/MeshAsset.h"
#include "Entity/Assets/TextureAsset.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

// Struct to associate a Modelpath with Material
struct Model
{
    Model() : meshPath(""), material(nullptr) {}
    Model(std::string& path, const Ref<MaterialAsset>& materialAsset) : meshPath(path), material(materialAsset) {}

    std::string meshPath;
    Ref<MaterialAsset> material;
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
    Model* LoadMeshAndTextures(std::string& path, Ref<MeshAsset>& mesh);
    Model* GetModel(const char* path);
    Ref<MaterialAsset> GetMaterial(const char* name);
    Ref<MaterialAsset> GetMaterial(uint32_t id);
    Ref<MaterialAsset> CreateMaterial();
    void RemoveMaterial(uint32_t id);
    std::vector<uint32_t> GetMaterialIds(bool includeDefault) const;

private:
    AssetManager();

    Scope<Assimp::Importer> m_Importer;
    std::unordered_map<std::string, Ref<MeshAsset>> m_LoadedMeshAssets;
    std::unordered_map<std::string, Ref<TextureAsset>> m_LoadedTextureAssets;
    std::unordered_map<std::string, Ref<Shader>> m_LoadedShaders;
    std::unordered_map<std::string, Model> m_LoadedModels;
    std::unordered_map<uint32_t, Ref<MaterialAsset>> m_LoadedMaterialAssets;

    void loadDefaultMeshAndTextures();
    void processNode(const aiNode* node, const aiScene* scene, std::vector<Ref<SubMesh>>& subMeshes);
    Ref<SubMesh> processMesh(aiMesh* mesh, const aiScene* scene);
};
