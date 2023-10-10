#include "AssetManager.h"

#include <ranges>

#include "IdManager.h"
#include "json.hpp"
#include "stb_image.h"
#include "stb_image_resize.h"
#include "assimp/postprocess.h"

AssetManager::AssetManager()
{
    m_Importer = CreateScope<Assimp::Importer>();
    loadDefaultMeshAndTextures();
}

void AssetManager::Reset()
{
    m_LoadedMeshAssets.clear();
    m_LoadedTextureAssets.clear();
    m_LoadedModels.clear();
    m_LoadedMaterialAssets.clear();
    m_Importer = CreateScope<Assimp::Importer>();
    loadDefaultMeshAndTextures();
}

MeshAsset* AssetManager::LoadMesh(const std::string& path)
{
    if (m_LoadedMeshAssets.contains(path))
    {
        return m_LoadedMeshAssets[path].get();
    }

    const aiScene* scene = m_Importer->ReadFile(
        path,
        aiProcess_FlipUVs | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_SortByPType |
            aiProcess_RemoveRedundantMaterials | aiProcess_FixInfacingNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_DEBUG(std::string("ERROR::ASSIMP::") + m_Importer->GetErrorString());
        return nullptr;
    }
    const SubModel tempSubModel;
    std::vector tempVector = {tempSubModel};
    processNode(scene->mRootNode, scene, tempVector, path);

    return tempSubModel.mesh;
}

MeshAsset* AssetManager::GetMesh(const uint32_t id)
{
    for (const auto& meshAsset : m_LoadedMeshAssets | std::views::values)
    {
        if (meshAsset->GetId() == id)
            return meshAsset.get();
    }
    return nullptr;
}

TextureAsset* AssetManager::LoadTexture(std::string& path, bool flipVertical, bool loadOnlyOneChannel, uint32_t channelIndex)
{
    if (loadOnlyOneChannel)
    {
        std::string fileName = path.substr(0, path.find_last_of('.'));
        const std::string fileEnding = path.substr(path.find_last_of('.'), path.size());
        fileName += "_@" + std::to_string(channelIndex);
        path = fileName + fileEnding;
    }

    const bool textureExists = m_LoadedTextureAssets.contains(path);
    if (textureExists && m_LoadedTextureAssets[path]->GetFlipVertical() == flipVertical)
        return m_LoadedTextureAssets[path].get();

    std::string pathToUse = path;
    if (path.find_last_of('@') != std::string::npos)
    {
        loadOnlyOneChannel = true;
        channelIndex = std::stoi(path.substr(path.find_last_of('@') + 1, path.size()));
        pathToUse = path.substr(0, path.find_last_of('@') - 1) + path.substr(path.find_last_of('@') + 2, path.size());
    }

    m_LoadedTextureAssets[path] = CreateScope<TextureAsset>(IdManager::GetInstance().CreateNewId(),
                                                            pathToUse,
                                                               flipVertical, loadOnlyOneChannel, channelIndex);
    const auto textureAsset = m_LoadedTextureAssets[path].get();

    stbi_set_flip_vertically_on_load(textureAsset->GetFlipVertical());

    importTexture(textureAsset);

    return textureAsset;
}

void AssetManager::ReloadTexture(TextureAsset* textureAsset)
{
    importTexture(textureAsset);
}

TextureAsset* AssetManager::GetTexture(const uint32_t id)
{
    for (const auto& textureAsset : m_LoadedTextureAssets | std::views::values)
    {
        if (textureAsset->GetId() == id)
            return textureAsset.get();
    }

    return nullptr;
}

Shader* AssetManager::LoadShader(const std::string& path, ShaderType shaderType)
{
    if (m_LoadedShaders.contains(path))
    {
        return m_LoadedShaders[path].get();
    }

    m_LoadedShaders[path] = CreateScope<Shader>(path.c_str(), shaderType);
    return m_LoadedShaders[path].get();
}

Model* AssetManager::LoadModel(const std::string& path)
{
    if (m_LoadedModels.contains(path))
    {
        return m_LoadedModels[path].get();
    }

    const aiScene* scene = m_Importer->ReadFile(
        path,
        aiProcess_FlipUVs | aiProcess_OptimizeMeshes | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_SortByPType |
            aiProcess_RemoveRedundantMaterials | aiProcess_FixInfacingNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_DEBUG(std::string("ERROR::ASSIMP::") + m_Importer->GetErrorString());

        return nullptr;
    }

    m_LoadedModels[path] = CreateScope<Model>();
    Model* model = m_LoadedModels[path].get();
    processNode(scene->mRootNode, scene, model->subModels, path);
    model->name = path.substr(path.find_last_of('/')+1, path.find_last_of('.'));

    return model;
}

Model* AssetManager::GetModel(const std::string& path)
{
    if (m_LoadedModels.contains(path))
        return m_LoadedModels[path].get();

    return nullptr;
}

MaterialAsset* AssetManager::GetMaterial(const std::string& name)
{
    for (const auto& asset : m_LoadedMaterialAssets | std::views::values)
    {
        if (asset->GetName() == name)
            return asset.get();
    }

    return nullptr;
}

MaterialAsset* AssetManager::GetMaterial(uint32_t id)
{
    if (m_LoadedMaterialAssets.contains(id))
        return m_LoadedMaterialAssets[id].get();

    return nullptr;
}

MaterialAsset* AssetManager::CreateMaterial()
{
    const uint32_t id = IdManager::GetInstance().CreateNewId();
    m_LoadedMaterialAssets[id] = CreateScope<MaterialAsset>(id, "New Material");
    const auto newMaterial = m_LoadedMaterialAssets[id].get();
    newMaterial->GetDiffusePath() = "default";
    *newMaterial->GetDiffuseTextureAsset() = m_LoadedTextureAssets["default"].get();

    return newMaterial;
}

void AssetManager::RemoveMaterial(uint32_t id)
{
    m_LoadedMaterialAssets.erase(id);
}

std::vector<uint32_t> AssetManager::GetMaterialIds(bool includeDefault) const
{
    std::vector<uint32_t> returnVector;
    for (auto& it : m_LoadedMaterialAssets)
    {
        if (!includeDefault && it.second->GetName() == "Default")
            continue;

        returnVector.push_back(it.first);
    }

    return returnVector;
}

std::vector<std::pair<std::string, Model*>> AssetManager::GetModels() const
{
    std::vector<std::pair<std::string, Model*>> returnVector;
    for (const auto& model : m_LoadedModels)
    {
        std::pair pair = {model.first, model.second.get()};
        returnVector.emplace_back(pair);
    }

    return returnVector;
}

std::vector<std::pair<std::string, MeshAsset*>> AssetManager::GetMeshes() const
{
    std::vector<std::pair<std::string, MeshAsset*>> returnVector;
    for (const auto& mesh : m_LoadedMeshAssets)
    {
        if (mesh.first == "default")
            continue;

        std::pair pair = {mesh.first, mesh.second.get()};
        returnVector.emplace_back(pair);
    }

    return returnVector;
}

std::vector<std::pair<uint32_t, MaterialAsset*>> AssetManager::GetMaterials() const
{
    std::vector<std::pair<uint32_t, MaterialAsset*>> returnVector;
    for (const auto& material : m_LoadedMaterialAssets)
    {
        if (material.second->GetName() == "Default")
            continue;

        std::pair pair = { material.first, material.second.get() };
        returnVector.push_back(pair);
    }

    return returnVector;
}

std::vector<std::pair<std::string, TextureAsset*>> AssetManager::GetTextures() const
{
    std::vector<std::pair<std::string, TextureAsset*>> returnVector;
    for (const auto& texture : m_LoadedTextureAssets)
    {
        if (texture.first == "default" || texture.first == "white" || texture.first == "black")
            continue;

        std::pair pair = { texture.first, texture.second.get() };
        returnVector.emplace_back(pair);
    }

    return returnVector;
}

void AssetManager::AddTexture(const std::string& path, Scope<TextureAsset>&& textureAsset)
{
    m_LoadedTextureAssets[path] = std::move(textureAsset);
}

void AssetManager::AddMesh(const std::string& path, Scope<MeshAsset>&& meshAsset)
{
    m_LoadedMeshAssets[path] = std::move(meshAsset);
}

void AssetManager::AddMaterial(Scope<MaterialAsset>&& materialAsset)
{
    m_LoadedMaterialAssets[materialAsset->GetId()] = std::move(materialAsset);
}

void AssetManager::AddModel(const std::string& path, Scope<Model>&& model)
{
    m_LoadedModels[path] = std::move(model);
}

TextureAsset* AssetManager::LoadTextureInternal(std::string& path, bool flipVertical, uint32_t assignedId,
    bool loadOnlyOneChannel, uint32_t channelIndex)
{
    if (loadOnlyOneChannel)
    {
        std::string fileName = path.substr(0, path.find_last_of('.'));
        const std::string fileEnding = path.substr(path.find_last_of('.'), path.size());
        fileName += "_@" + std::to_string(channelIndex);
        path = fileName + fileEnding;
    }

    const bool textureExists = m_LoadedTextureAssets.contains(path);
    if (textureExists && m_LoadedTextureAssets[path]->GetFlipVertical() == flipVertical)
        return m_LoadedTextureAssets[path].get();

    std::string pathToUse = path;
    if (path.find_last_of('@') != std::string::npos)
    {
        loadOnlyOneChannel = true;
        channelIndex = std::stoi(path.substr(path.find_last_of('@') + 1, path.size()));
        pathToUse = path.substr(0, path.find_last_of('@') - 1) + path.substr(path.find_last_of('@') + 2, path.size());
    }

    m_LoadedTextureAssets[path] = CreateScope<TextureAsset>(assignedId, pathToUse,
                                                            flipVertical, loadOnlyOneChannel, channelIndex);
    const auto textureAsset = m_LoadedTextureAssets[path].get();

    stbi_set_flip_vertically_on_load(textureAsset->GetFlipVertical());

    importTexture(textureAsset);

    return textureAsset;
}

void AssetManager::importTexture(TextureAsset* textureAsset)
{
    const std::string pathToUse = textureAsset->GetPath();
    unsigned char* loadedData = stbi_load(pathToUse.c_str(), textureAsset->GetWidth(), textureAsset->GetHeight(),
                                          textureAsset->GetNrComponents(), 0);

    unsigned char* resizedData = nullptr;
    if (*textureAsset->GetWidth() > 4000 && *textureAsset->GetHeight() > 4000)
    {
        resizedData = new unsigned char[1920 * 1080 * *textureAsset->GetNrComponents()];
        stbir_resize_uint8(loadedData, *textureAsset->GetWidth(), *textureAsset->GetHeight(), 0, resizedData,
                                   1920, 1080, 0, *textureAsset->GetNrComponents());
        *textureAsset->GetWidth() = 1920;
        *textureAsset->GetHeight() = 1080;
        stbi_image_free(loadedData);
    }

    if (textureAsset->GetLoadOnlyOneChannel())
    {
        const size_t dataSize = (*textureAsset->GetWidth()) * (*textureAsset->GetHeight());
        auto* data = new unsigned char[dataSize];

        for (int32_t y = 0; y < *textureAsset->GetHeight(); y++)
        {
            for (int32_t x = 0; x < *textureAsset->GetWidth(); x++)
            {
                const unsigned char* pixelOffset =
                    *textureAsset->GetNrComponents() * (y * *textureAsset->GetWidth() + x) + (resizedData ? resizedData : loadedData);
                data[y * (*textureAsset->GetWidth()) + x] = pixelOffset[*textureAsset->GetChannelIndex()];
            }
        }
        *textureAsset->GetNrComponents() = 1;
        textureAsset->SetTextureData(data);

        stbi_image_free(resizedData ? resizedData : loadedData);
        delete[] data;
    }
    else
    {
        textureAsset->SetTextureData(resizedData ? resizedData : loadedData);
        stbi_image_free(resizedData ? resizedData : loadedData);
    }
}

void AssetManager::loadDefaultMeshAndTextures()
{
    // Setup default cube
    std::vector<MeshVertex> defaultVertices;

    //Back
    defaultVertices.push_back(
        {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    //Left
    defaultVertices.push_back(
        {{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    //Right
    defaultVertices.push_back(
        {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    //Front
    defaultVertices.push_back(
        {{-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    //Top
    defaultVertices.push_back(
        {{1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    //Bottom
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    std::vector<uint32_t> defaultIndices;

    const std::string defaultMeshPath = std::string("default");
    m_LoadedMeshAssets[defaultMeshPath] = CreateScope<MeshAsset>(0, defaultMeshPath, defaultVertices, defaultIndices);

    //Default "Prototype" Texture
    std::string defaultTexturePath("assets/textures/default.png");
    const auto defaultTextureAsset = LoadTextureInternal(defaultTexturePath, false, 1);
    auto nodeHandle = m_LoadedTextureAssets.extract("assets/textures/default.png");
    nodeHandle.key() = "default";
    m_LoadedTextureAssets.insert(std::move(nodeHandle));
    m_LoadedMaterialAssets[2] = CreateScope<MaterialAsset>(2, "Default");
    const auto defaultMaterial = m_LoadedMaterialAssets[2].get();
    defaultMaterial->GetDiffusePath() = "default";
    *defaultMaterial->GetDiffuseTextureAsset() = defaultTextureAsset;
 
    //White 1x1 Texture
    std::string whiteTexturePath("assets/textures/default_white.png");
    const auto whiteTextureAsset = LoadTextureInternal(whiteTexturePath, false, 3);
    auto nodeHandleWhite = m_LoadedTextureAssets.extract("assets/textures/default_white.png");
    nodeHandleWhite.key() = "white";
    m_LoadedTextureAssets.insert(std::move(nodeHandleWhite));

    //Black 1x1 Texture
    std::string blackTexturePath("assets/textures/default_black.png");
    const auto blackTextureAsset = LoadTextureInternal(blackTexturePath, false, 4);
    auto nodeHandleBlack = m_LoadedTextureAssets.extract("assets/textures/default_black.png");
    nodeHandleBlack.key() = "black";
    m_LoadedTextureAssets.insert(std::move(nodeHandleBlack));
}

void AssetManager::processNode(const aiNode* node, const aiScene* scene, std::vector<SubModel>& subModels, const std::string& path)
{
    SubModel subModelNode;
    subModelNode.name = node->mName.C_Str();
    aiMatrix4x4 modelMat = node->mTransformation;
    subModelNode.modelMatrix = {modelMat.a1, modelMat.b1, modelMat.c1, modelMat.d1,
                                   modelMat.a2, modelMat.b2, modelMat.c2, modelMat.d2,
                                   modelMat.a3, modelMat.b3, modelMat.c3, modelMat.d3,
                                   modelMat.a4, modelMat.b4, modelMat.c4, modelMat.d4};

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        SubModel subModelMesh;
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        subModelMesh.name = mesh->mName.C_Str();
        subModelMesh.mesh = processMesh(mesh, scene, path);
        processMaterials(scene, subModelMesh, path, mesh->mMaterialIndex);
        subModelNode.subModels.push_back(subModelMesh);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, subModelNode.subModels, path);
    }
    subModels.push_back(subModelNode);
}

MeshAsset* AssetManager::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& path)
{
    std::string meshPath = path + '@' + mesh->mName.C_Str();
    m_LoadedMeshAssets[meshPath] = CreateScope<MeshAsset>(IdManager::GetInstance().CreateNewId(), meshPath);

    auto& vertices = m_LoadedMeshAssets[meshPath]->GetVertices();
    auto& indices = m_LoadedMeshAssets[meshPath]->GetIndices();

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        MeshVertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // return a MeshAsset created from the extracted mesh data
    return m_LoadedMeshAssets[meshPath].get();
}

void AssetManager::processMaterials(const aiScene* scene, SubModel& subModel, const std::string& path, const uint32_t materialIndex)
{
    const std::string directory = path.substr(0, path.find_last_of('/'));
    const std::string materialName = path + '@' + std::to_string(materialIndex);
    MaterialAsset* materialAsset = GetMaterial(materialName);
    if (materialAsset)
    {
        subModel.material = materialAsset;
        return;
    }

    const uint32_t materialAssetId = IdManager::GetInstance().CreateNewId();
    m_LoadedMaterialAssets[materialAssetId] = CreateScope<MaterialAsset>(materialAssetId, materialName);
    materialAsset = m_LoadedMaterialAssets[materialAssetId].get();

    auto& diffusePath = materialAsset->GetDiffusePath();
    auto& normalPath = materialAsset->GetNormalPath();
    auto& metallicPath = materialAsset->GetMetallicPath();
    auto& roughnessPath = materialAsset->GetRoughnessPath();
    auto& aoPath = materialAsset->GetAOPath();
    auto& emissivePath = materialAsset->GetEmissivePath();

    const aiMaterial* aiMaterial = scene->mMaterials[materialIndex];

    // 1. diffuse map
    if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE))
    {
        aiString diffuseFile;
        aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseFile);
        diffusePath = directory + '/' + diffuseFile.C_Str();
    }
    // 2. normal maps
    if (aiMaterial->GetTextureCount(aiTextureType_NORMALS))
    {
        aiString normalFile;
        aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &normalFile);
        normalPath = directory + '/' + normalFile.C_Str();
    }
    else if (aiMaterial->GetTextureCount(aiTextureType_HEIGHT))
    {
        aiString normalFile;
        aiMaterial->GetTexture(aiTextureType_HEIGHT, 0, &normalFile);
        normalPath = directory + '/' + normalFile.C_Str();
    }
    // 3. metallic map
    if (aiMaterial->GetTextureCount(aiTextureType_METALNESS))
    {
        aiString metallicFile;
        aiMaterial->GetTexture(aiTextureType_METALNESS, 0, &metallicFile);
        metallicPath = directory + '/' + metallicFile.C_Str();
    }
    // 4. roughness map
    if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS))
    {
        aiString roughnessFile;
        aiMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &roughnessFile);
        roughnessPath = directory + '/' + roughnessFile.C_Str();
    }
    // 4. AO map
    if (aiMaterial->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION))
    {
        aiString aoFile;
        aiMaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &aoFile);
        aoPath = directory + '/' + aoFile.C_Str();
    }
    // 5. Emissive map
    if (aiMaterial->GetTextureCount(aiTextureType_EMISSIVE))
    {
        aiString emissiveFile;
        aiMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &emissiveFile);
        emissivePath = directory + '/' + emissiveFile.C_Str();
    }

    const bool metalRoughnessIsShared = metallicPath == roughnessPath;
    if (!diffusePath.empty())
        *materialAsset->GetDiffuseTextureAsset() = LoadTexture(diffusePath, materialAsset->GetFlipDiffuseTexture());
    if (!normalPath.empty())
        *materialAsset->GetNormalTextureAsset() = LoadTexture(normalPath, materialAsset->GetFlipNormalTexture());
    if (!metallicPath.empty())
        *materialAsset->GetMetallicTextureAsset() = LoadTexture(metallicPath, materialAsset->GetFlipMetallicTexture(), metalRoughnessIsShared, 2);
    if (!roughnessPath.empty())
        *materialAsset->GetRoughnessTextureAsset() = LoadTexture(roughnessPath, materialAsset->GetFlipRoughnessTexture(), metalRoughnessIsShared, 1);
    if (!aoPath.empty())
        *materialAsset->GetAOTextureAsset() = LoadTexture(aoPath, materialAsset->GetFlipAOTexture());
    if (!emissivePath.empty())
        *materialAsset->GetEmissiveTextureAsset() = LoadTexture(emissivePath, materialAsset->GetFlipEmissiveTexture());

    subModel.material = materialAsset;
}

nlohmann::ordered_json Model::SerializeObject()
{
    nlohmann::ordered_json model = {
        {"Name", name},
        {"SubModels", addSubModelJson(subModels)}
    };

    return model;
}

void Model::DeserializeObject(nlohmann::json jsonObject)
{
    name = jsonObject["Name"];
    subModels = deserializeSubModels(jsonObject["SubModels"]);
}

std::vector<SubModel>&& Model::deserializeSubModels(nlohmann::json subModelsJsonArr)
{
    std::vector<SubModel> returnVector;
    for (nlohmann::json subModelJson : subModelsJsonArr)
    {
        SubModel subModel;
        subModel.name = subModelJson["Name"];
        subModel.modelMatrix = glm::mat4(1.0f);
        subModel.modelMatrix[0][0] = subModelJson["ModelMatrix"]["0"];
        subModel.modelMatrix[0][1] = subModelJson["ModelMatrix"]["1"];
        subModel.modelMatrix[0][2] = subModelJson["ModelMatrix"]["2"];
        subModel.modelMatrix[0][3] = subModelJson["ModelMatrix"]["3"];
        subModel.modelMatrix[1][0] = subModelJson["ModelMatrix"]["4"];
        subModel.modelMatrix[1][1] = subModelJson["ModelMatrix"]["5"];
        subModel.modelMatrix[1][2] = subModelJson["ModelMatrix"]["6"];
        subModel.modelMatrix[1][3] = subModelJson["ModelMatrix"]["7"];
        subModel.modelMatrix[2][0] = subModelJson["ModelMatrix"]["8"];
        subModel.modelMatrix[2][1] = subModelJson["ModelMatrix"]["9"];
        subModel.modelMatrix[2][2] = subModelJson["ModelMatrix"]["10"];
        subModel.modelMatrix[2][3] = subModelJson["ModelMatrix"]["11"];
        subModel.modelMatrix[3][0] = subModelJson["ModelMatrix"]["12"];
        subModel.modelMatrix[3][1] = subModelJson["ModelMatrix"]["13"];
        subModel.modelMatrix[3][2] = subModelJson["ModelMatrix"]["14"];
        subModel.modelMatrix[3][3] = subModelJson["ModelMatrix"]["15"];
        if (subModelJson.contains("MeshAssetId"))
        {
            const uint32_t meshAssetId = subModelJson["MeshAssetId"];
            subModel.mesh = AssetManager::GetInstance().GetMesh(meshAssetId);
        }
        if (subModelJson.contains("MaterialAssetId"))
        {
            const uint32_t materialAssetId = subModelJson["MaterialAssetId"];
            subModel.material = AssetManager::GetInstance().GetMaterial(materialAssetId);
        }
        if (subModelJson.contains("SubModels"))
            subModel.subModels = deserializeSubModels(subModelJson["SubModels"]);

        returnVector.push_back(subModel);
    }

    return std::move(returnVector);
}

nlohmann::ordered_json Model::addSubModelJson(std::vector<SubModel>& subModels)
{
    nlohmann::ordered_json subModelsArray = nlohmann::json::array();

    uint32_t i = 0;
    for (auto& subModel : subModels)
    {
        subModelsArray[i] = {{"Name", subModel.name},
                             {"ModelMatrix",
                                {{"0", subModel.modelMatrix[0][0]},
                                {"1", subModel.modelMatrix[0][1]},
                                {"2", subModel.modelMatrix[0][2]},
                                {"3", subModel.modelMatrix[0][3]},
                                {"4", subModel.modelMatrix[1][0]},
                                {"5", subModel.modelMatrix[1][1]},
                                {"6", subModel.modelMatrix[1][2]},
                                {"7", subModel.modelMatrix[1][3]},
                                {"8", subModel.modelMatrix[2][0]},
                                {"9", subModel.modelMatrix[2][1]},
                                {"10", subModel.modelMatrix[2][2]},
                                {"11", subModel.modelMatrix[2][3]},
                                {"12", subModel.modelMatrix[3][0]},
                                {"13", subModel.modelMatrix[3][1]},
                                {"14", subModel.modelMatrix[3][2]},
                                {"15", subModel.modelMatrix[3][3]}}}};
        if (subModel.mesh)
            subModelsArray[i]["MeshAssetId"] = subModel.mesh->GetId();
        if (subModel.material)
            subModelsArray[i]["MaterialAssetId"] = subModel.material->GetId();
        nlohmann::ordered_json nextSubModels = addSubModelJson(subModel.subModels);
        if (!nextSubModels.empty())
        {
            subModelsArray[i]["SubModels"] = nlohmann::json::array();
            subModelsArray[i]["SubModels"] = nextSubModels;
        }

        i++;
    }

    return subModelsArray;
}
