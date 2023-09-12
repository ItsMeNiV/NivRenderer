#include "AssetManager.h"

#include <ranges>

#include "IdManager.h"
#include "stb_image.h"

AssetManager::AssetManager()
{
    m_Importer = CreateScope<Assimp::Importer>();
    loadDefaultMeshAndTextures();
}

Ref<MeshAsset> AssetManager::LoadMesh(const std::string& path)
{
    if (m_LoadedMeshAssets.contains(path))
    {
        return m_LoadedMeshAssets[path];
    }

    const aiScene* scene = m_Importer->ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_DEBUG(std::string("ERROR::ASSIMP::") + m_Importer->GetErrorString());
        return nullptr;
    }
    SubModel tempSubModel;
    std::vector tempVector = {tempSubModel};
    processNode(scene->mRootNode, scene, tempVector, path);

    return tempSubModel.mesh;
}

Ref<TextureAsset> AssetManager::LoadTexture(std::string& path, bool flipVertical, bool loadOnlyOneChannel, int channelIndex)
{
    const bool textureExists = m_LoadedTextureAssets.contains(path);
    if (textureExists && m_LoadedTextureAssets[path]->GetFlipVertical() == flipVertical)
        return m_LoadedTextureAssets[path];

    std::string pathToUse = textureExists ? m_LoadedTextureAssets[path]->GetPath() : path;

    Ref<TextureAsset> textureAsset = CreateRef<TextureAsset>(IdManager::GetInstance().CreateNewId(), pathToUse, flipVertical);

    stbi_set_flip_vertically_on_load(textureAsset->GetFlipVertical());
    if (loadOnlyOneChannel)
    {
        unsigned char* loadedData = stbi_load(pathToUse.c_str(), textureAsset->GetWidth(), textureAsset->GetHeight(), textureAsset->GetNrComponents(), 0);
        const size_t dataSize = (*textureAsset->GetWidth()) * (*textureAsset->GetHeight());
        auto* data = new unsigned char[dataSize];

        for (int32_t y = 0; y < *textureAsset->GetHeight(); y++)
        {
            for (int32_t x = 0; x < *textureAsset->GetWidth(); x++)
            {

                const unsigned char* pixelOffset = *textureAsset->GetNrComponents() * (y * *textureAsset->GetWidth() + x) + loadedData;
                data[y * (*textureAsset->GetWidth()) + x] = pixelOffset[channelIndex];
            }
        }
        *textureAsset->GetNrComponents() = 1;
        textureAsset->SetTextureData(data);

        stbi_image_free(loadedData);
        delete[] data;

        std::string fileName = path.substr(0, path.find_last_of('.'));
        const std::string fileEnding = path.substr(path.find_last_of('.'), path.size());
        fileName += "_@" + std::to_string(channelIndex);
        path = fileName + fileEnding;
    }
    else
    {
        unsigned char* loadedData = stbi_load(pathToUse.c_str(), textureAsset->GetWidth(), textureAsset->GetHeight(),
                                              textureAsset->GetNrComponents(), 0);
        textureAsset->SetTextureData(loadedData);
        stbi_image_free(loadedData);
    }

    m_LoadedTextureAssets[path] = textureAsset;

    return textureAsset;
}

Ref<Shader> AssetManager::LoadShader(const std::string& path, ShaderType shaderType)
{
    if (m_LoadedShaders.contains(path))
    {
        return m_LoadedShaders[path];
    }

    Ref<Shader> shader = CreateRef<Shader>(path.c_str(), shaderType);
    m_LoadedShaders[path] = shader;
    return shader;
}

Ref<Model> AssetManager::LoadModel(const std::string& path)
{
    if (m_LoadedModels.contains(path))
    {
        return m_LoadedModels[path];
    }

    const aiScene* scene = m_Importer->ReadFile(
        path,
        aiProcess_FlipUVs | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_SortByPType |
            aiProcess_RemoveRedundantMaterials | aiProcess_FixInfacingNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_DEBUG(std::string("ERROR::ASSIMP::") + m_Importer->GetErrorString());

        return nullptr;
    }

    m_LoadedModels[path] = CreateRef<Model>();
    Ref<Model> model = m_LoadedModels[path];
    processNode(scene->mRootNode, scene, model->subModels, path);
    model->name = path.substr(path.find_last_of('/')+1, path.find_last_of('.'));

    return model;
}

Ref<Model> AssetManager::GetModel(const std::string& path)
{
    if (m_LoadedModels.contains(path))
        return m_LoadedModels[path];

    return nullptr;
}

Ref<MaterialAsset> AssetManager::GetMaterial(const std::string& name)
{
    for (auto& asset : m_LoadedMaterialAssets | std::views::values)
    {
        if (asset->GetName() == name)
            return asset;
    }

    return nullptr;
}

Ref<MaterialAsset> AssetManager::GetMaterial(uint32_t id)
{
    if (m_LoadedMaterialAssets.contains(id))
        return m_LoadedMaterialAssets[id];

    return nullptr;
}

Ref<MaterialAsset> AssetManager::CreateMaterial()
{
    const Ref<MaterialAsset> newMaterial = CreateRef<MaterialAsset>(IdManager::GetInstance().CreateNewId(), "New Material");
    newMaterial->GetDiffusePath() = "default";
    newMaterial->GetDiffuseTextureAsset() = m_LoadedTextureAssets["default"];
    m_LoadedMaterialAssets[newMaterial->GetId()] = newMaterial;

    return m_LoadedMaterialAssets[newMaterial->GetId()];
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

std::vector<std::pair<std::string, Ref<Model>>> AssetManager::GetModels() const
{
    std::vector<std::pair<std::string, Ref<Model>>> returnVector;
    for (const auto& model : m_LoadedModels)
    {
        returnVector.emplace_back(model);
    }

    return returnVector;
}

std::vector<std::pair<std::string, Ref<MeshAsset>>> AssetManager::GetMeshes() const
{
    std::vector<std::pair<std::string, Ref<MeshAsset>>> returnVector;
    for (const auto& mesh : m_LoadedMeshAssets)
    {
        returnVector.emplace_back(mesh);
    }

    return returnVector;
}

std::vector<std::pair<uint32_t, Ref<MaterialAsset>>> AssetManager::GetMaterials() const
{
    std::vector<std::pair<uint32_t, Ref<MaterialAsset>>> returnVector;
    for (const auto& material : m_LoadedMaterialAssets)
    {
        returnVector.push_back(material);
    }

    return returnVector;
}

std::vector<std::pair<std::string, Ref<TextureAsset>>> AssetManager::GetTextures() const
{
    std::vector<std::pair<std::string, Ref<TextureAsset>>> returnVector;
    for (const auto& texture : m_LoadedTextureAssets)
    {
        returnVector.emplace_back(texture);
    }

    return returnVector;
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
    const Ref<MeshAsset> defaultMeshAsset = CreateRef<MeshAsset>(IdManager::GetInstance().CreateNewId(), defaultMeshPath, defaultVertices, defaultIndices);
    m_LoadedMeshAssets[defaultMeshPath] = defaultMeshAsset;

    //Default "Prototype" Texture
    std::string defaultTexturePath("assets/textures/default.png");
    const Ref<TextureAsset> defaultTextureAsset = LoadTexture(defaultTexturePath, false);
    auto nodeHandle = m_LoadedTextureAssets.extract("assets/textures/default.png");
    nodeHandle.key() = "default";
    m_LoadedTextureAssets.insert(std::move(nodeHandle));
    const Ref<MaterialAsset> defaultMaterial = CreateRef<MaterialAsset>(IdManager::GetInstance().CreateNewId(), "Default");
    defaultMaterial->GetDiffusePath() = "default";
    defaultMaterial->GetDiffuseTextureAsset() = defaultTextureAsset;
    m_LoadedMaterialAssets[defaultMaterial->GetId()] = defaultMaterial;
 
    //White 1x1 Texture
    std::string path("assets/textures/default_white.png");
    const Ref<TextureAsset> whiteTextureAsset = LoadTexture(path, false);
    auto nodeHandleWhite = m_LoadedTextureAssets.extract("assets/textures/default_white.png");
    nodeHandleWhite.key() = "white";
    m_LoadedTextureAssets.insert(std::move(nodeHandleWhite));

    //Black 1x1 Texture
    path = "assets/textures/default_black.png";
    const Ref<TextureAsset> blackTextureAsset = LoadTexture(path, false);
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

Ref<MeshAsset> AssetManager::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& path)
{
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

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
    std::string meshPath = path + '@' + mesh->mName.C_Str();
    auto meshAsset = CreateRef<MeshAsset>(IdManager::GetInstance().CreateNewId(), meshPath, vertices, indices);
    m_LoadedMeshAssets[meshPath] = meshAsset;
    return meshAsset;
}

void AssetManager::processMaterials(const aiScene* scene, SubModel& subModel, const std::string& path, const uint32_t materialIndex)
{
    const std::string directory = path.substr(0, path.find_last_of('/'));
    const std::string materialName = path + '@' + std::to_string(materialIndex);
    Ref<MaterialAsset> materialAsset = GetMaterial(materialName);
    if (materialAsset)
    {
        subModel.material = materialAsset;
        return;
    }

    materialAsset = CreateRef<MaterialAsset>(IdManager::GetInstance().CreateNewId(), materialName);
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
        materialAsset->GetDiffuseTextureAsset() = LoadTexture(diffusePath, materialAsset->GetFlipDiffuseTexture());
    if (!normalPath.empty())
        materialAsset->GetNormalTextureAsset() = LoadTexture(normalPath, materialAsset->GetFlipNormalTexture());
    if (!metallicPath.empty())
        materialAsset->GetMetallicTextureAsset() =
            LoadTexture(metallicPath, materialAsset->GetFlipMetallicTexture(), metalRoughnessIsShared, 2);
    if (!roughnessPath.empty())
        materialAsset->GetRoughnessTextureAsset() =
            LoadTexture(roughnessPath, materialAsset->GetFlipRoughnessTexture(), metalRoughnessIsShared, 1);
    if (!aoPath.empty())
        materialAsset->GetAOTextureAsset() = LoadTexture(aoPath, materialAsset->GetFlipAOTexture());
    if (!emissivePath.empty())
        materialAsset->GetEmissiveTextureAsset() = LoadTexture(emissivePath, materialAsset->GetFlipEmissiveTexture());

    m_LoadedMaterialAssets[materialAsset->GetId()] = materialAsset;

    subModel.material = materialAsset;
}

ordered_json Model::SerializeObject()
{
    ordered_json model = {
        {"Name", name},
        {"SubModels", addSubModelJson(subModels)}
    };

    return model;
}

ordered_json Model::addSubModelJson(std::vector<SubModel>& subModels)
{
    ordered_json subModelsArray = json::array();

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
        ordered_json nextSubModels = addSubModelJson(subModel.subModels);
        if (!nextSubModels.empty())
        {
            subModelsArray[i]["SubModels"] = json::array();
            subModelsArray[i]["SubModels"] = nextSubModels;
        }

        i++;
    }

    return subModelsArray;
}
