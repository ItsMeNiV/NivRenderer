#pragma once
#include "Base.h"
#include "MaterialAsset.h"
#include "Entity/Assets/MeshAsset.h"
#include "Entity/Assets/TextureAsset.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

struct SubModel
{
    SubModel() : mesh(nullptr), material(nullptr), modelMatrix(1.0f) {}

    std::string name;
    MeshAsset* mesh;
    MaterialAsset* material;
    std::vector<SubModel> subModels;
    glm::mat4 modelMatrix;
};
struct Model
{
    std::string name;
    std::vector<SubModel> subModels;

    ordered_json SerializeObject();
    void DeserializeObject(json jsonObject);
    static ordered_json addSubModelJson(std::vector<SubModel>& subModels);
    static std::vector<SubModel>&& deserializeSubModels(json subModelsJsonArr);
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

    MeshAsset* LoadMesh(const std::string& path);
    MeshAsset* GetMesh(const uint32_t id);
    TextureAsset* LoadTexture(std::string& path, bool flipVertical, bool loadOnlyOneChannel = false, int channelIndex = 0);
    void ReloadTexture(TextureAsset* textureAsset);
    TextureAsset* GetTexture(const uint32_t id);
    Shader* LoadShader(const std::string& path, ShaderType shaderType);
    Model* LoadModel(const std::string& path);
    Model* GetModel(const std::string& path);
    MaterialAsset* GetMaterial(const std::string& name);
    MaterialAsset* GetMaterial(uint32_t id);
    MaterialAsset* CreateMaterial();
    void RemoveMaterial(uint32_t id);
    std::vector<uint32_t> GetMaterialIds(bool includeDefault) const;

    // Used for Serialization
    std::vector<std::pair<std::string, Model*>> GetModels() const;
    std::vector<std::pair<std::string, MeshAsset*>> GetMeshes() const;
    std::vector<std::pair<uint32_t, MaterialAsset*>> GetMaterials() const;
    std::vector<std::pair<std::string, TextureAsset*>> GetTextures() const;
    //Used for Deserialization
    void AddTexture(const std::string& path, Scope<TextureAsset>&& textureAsset);
    void AddMesh(const std::string& path, Scope<MeshAsset>&& meshAsset);
    void AddMaterial(Scope<MaterialAsset>&& materialAsset);
    void AddModel(const std::string& path, Scope<Model>&& model);

private:
    AssetManager();

    Scope<Assimp::Importer> m_Importer;
    std::unordered_map<std::string, Scope<MeshAsset>> m_LoadedMeshAssets;
    std::unordered_map<std::string, Scope<TextureAsset>> m_LoadedTextureAssets;
    std::unordered_map<std::string, Scope<Shader>> m_LoadedShaders;
    std::unordered_map<std::string, Scope<Model>> m_LoadedModels;
    std::unordered_map<uint32_t, Scope<MaterialAsset>> m_LoadedMaterialAssets;

    void importTexture(TextureAsset* textureAsset);
    void loadDefaultMeshAndTextures();
    void processNode(const aiNode* node, const aiScene* scene, std::vector<SubModel>& subModels, const std::string& path);
    MeshAsset* processMesh(aiMesh* mesh, const aiScene* scene, const std::string& path);
    void processMaterials(const aiScene* scene, SubModel& subModel, const std::string& path, const uint32_t materialIndex);
};
