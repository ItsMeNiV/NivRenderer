#pragma once
#include "Base.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "Assets/Assets.h"
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
};

class NewAssetManager
{
public:
    NewAssetManager(NewAssetManager const&) = delete;
    void operator=(NewAssetManager const&) = delete;

    static NewAssetManager& GetInstance()
    {
        static NewAssetManager instance;
        return instance;
    }

    void Reset();

    //TODO: Check where Load methods are wrongfully used
    ShaderAsset* LoadShader(const std::string& path, ShaderType shaderType);
    ShaderAsset* GetShader(uint32_t id);
    ShaderAsset* GetShader(const std::string& path);
    MeshAsset* LoadMesh(const std::string& path);
    MeshAsset* GetMesh(uint32_t id);
    MeshAsset* GetMesh(const std::string& path);
    TextureAsset* LoadTexture(std::string& path, bool flipVertical, bool loadOnlyOneChannel = false, uint32_t channelIndex = 0);
    TextureAsset* GetTexture(uint32_t id);
    TextureAsset* GetTexture(const std::string& path);
    std::vector<uint32_t> GetTextureIds(bool includeDefault) const;
    MaterialAsset* GetMaterial(const std::string& name);
    MaterialAsset* GetMaterial(uint32_t id);
    MaterialAsset* CreateMaterial();
    void RemoveMaterial(uint32_t id);
    std::vector<uint32_t> GetMaterialIds(bool includeDefault) const;
    Model* LoadModel(const std::string& path);
    
private:
    NewAssetManager();

    Scope<Assimp::Importer> m_Importer;

    std::unordered_map<uint32_t, Scope<ShaderAsset>> m_LoadedShaders;
    std::unordered_map<std::string, ShaderAsset*> m_ShadersByPath;
    std::unordered_map<uint32_t, Scope<MeshAsset>> m_LoadedMeshes;
    std::unordered_map<std::string, MeshAsset*> m_MeshesByPath;
    std::unordered_map<uint32_t, Scope<MaterialAsset>> m_LoadedMaterials;
    std::unordered_map<std::string, MaterialAsset*> m_MaterialsByName;
    std::unordered_map<uint32_t, Scope<TextureAsset>> m_LoadedTextures;
    std::unordered_map<std::string, TextureAsset*> m_TexturesByPath;

    static constexpr int MESH_IMPORT_POSTPROCESS_FLAGS = aiProcess_FlipUVs | aiProcess_OptimizeMeshes |
        aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_SortByPType | aiProcess_RemoveRedundantMaterials |
        aiProcess_FixInfacingNormals;

    void loadDefaults();
    static void importTexture(TextureAsset* textureAsset);
    void processNode(const aiNode* node, const aiScene* scene, std::vector<SubModel>& subModels, const std::string& path);
    MeshAsset* processMesh(aiMesh* mesh, const aiScene* scene, const std::string& path);
    void processMaterials(const aiScene* scene, SubModel& subModel, const std::string& path, const uint32_t materialIndex);
};