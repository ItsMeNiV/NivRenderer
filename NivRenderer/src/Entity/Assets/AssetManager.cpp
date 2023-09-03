#include "AssetManager.h"
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

    std::vector<Ref<SubMesh>> subMeshes;

    const aiScene* scene = m_Importer->ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_DEBUG(std::string("ERROR::ASSIMP::") + m_Importer->GetErrorString());
        return nullptr;
    }
    processNode(scene->mRootNode, scene, subMeshes);

    Ref<MeshAsset> meshAsset = CreateRef<MeshAsset>(IdManager::GetInstance().CreateNewId(), path);
    meshAsset->AddSubMeshes(subMeshes);
    m_LoadedMeshAssets[path] = meshAsset;

    return meshAsset;
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
        textureAsset->SetTextureData(stbi_load(pathToUse.c_str(), textureAsset->GetWidth(), textureAsset->GetHeight(),
                                               textureAsset->GetNrComponents(), 0));
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

std::vector<std::string> AssetManager::LoadMeshAndTextures(
    std::string& path, Ref<MeshAsset>& mesh, Ref<TextureAsset>& diffuseTexture, Ref<TextureAsset>& normalTexture,
    Ref<TextureAsset>& metallicTexture, Ref<TextureAsset>& roughnessTexture, Ref<TextureAsset>& aoTexture,
    Ref<TextureAsset>& emissiveTexture, bool flipVerticalDiffuse, bool flipVerticalNormal, bool flipVerticalMetallic,
    bool flipVerticalRoughness, bool flipVerticalAO, bool flipVerticalEmissive)
{
    std::vector<std::string> returnPaths;

    if (m_LoadedModels.contains(path))
    {
        auto& m = m_LoadedModels[path];
        mesh = m_LoadedMeshAssets[path];

        if (!m.diffuseTexturePath.empty())
            diffuseTexture = LoadTexture(m.diffuseTexturePath, flipVerticalDiffuse);
        if (!m.normalTexturePath.empty())
            normalTexture = LoadTexture(m.normalTexturePath, flipVerticalNormal);
        if (!m.metallicTexturePath.empty())
            metallicTexture = LoadTexture(m.metallicTexturePath, flipVerticalMetallic);
        if (!m.roughnessTexturePath.empty())
            roughnessTexture = LoadTexture(m.roughnessTexturePath, flipVerticalRoughness);
        if (!m.aoTexturePath.empty())
            aoTexture = LoadTexture(m.aoTexturePath, flipVerticalAO);
        if (!m.emissiveTexturePath.empty())
            emissiveTexture = LoadTexture(m.emissiveTexturePath, flipVerticalEmissive);

        returnPaths.push_back(m.meshPath);
        returnPaths.push_back(m.diffuseTexturePath);
        returnPaths.push_back(m.normalTexturePath);
        returnPaths.push_back(m.metallicTexturePath);
        returnPaths.push_back(m.roughnessTexturePath);
        returnPaths.push_back(m.aoTexturePath);
        returnPaths.push_back(m.emissiveTexturePath);

        return returnPaths;
    }

    std::string directory = path.substr(0, path.find_last_of('/'));

    const aiScene* scene = m_Importer->ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_DEBUG(std::string("ERROR::ASSIMP::") + m_Importer->GetErrorString());
        return returnPaths;
    }

    Ref<MeshAsset> newMesh = CreateRef<MeshAsset>(IdManager::GetInstance().CreateNewId(), path);
    std::vector<Ref<SubMesh>> subMeshes;
    processNode(scene->mRootNode, scene, subMeshes);

    newMesh->AddSubMeshes(subMeshes);
    m_LoadedMeshAssets[path] = newMesh;
    mesh = newMesh;
    returnPaths.push_back(path);

    std::string diffusePath;
    std::string normalPath;
    std::string metallicPath;
    std::string roughnessPath;
    std::string aoPath;
    std::string emissivePath;
    for (uint32_t i = 0; i < scene->mNumMaterials; i++)
    {
        const aiMaterial* material = scene->mMaterials[i];

        // 1. diffuse map
        if (material->GetTextureCount(aiTextureType_DIFFUSE))
        {
            aiString diffuseFile;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseFile);
            diffusePath = directory + '/' + diffuseFile.C_Str();
        }
        // 2. normal maps
        if (material->GetTextureCount(aiTextureType_NORMALS))
        {
            aiString normalFile;
            material->GetTexture(aiTextureType_NORMALS, 0, &normalFile);
            normalPath = directory + '/' + normalFile.C_Str();
        }
        else if (material->GetTextureCount(aiTextureType_HEIGHT))
        {
            aiString normalFile;
            material->GetTexture(aiTextureType_HEIGHT, 0, &normalFile);
            normalPath = directory + '/' + normalFile.C_Str();
        }
        // 3. metallic map
        if (material->GetTextureCount(aiTextureType_METALNESS))
        {
            aiString metallicFile;
            material->GetTexture(aiTextureType_METALNESS, 0, &metallicFile);
            metallicPath = directory + '/' + metallicFile.C_Str();
        }
        // 4. roughness map
        if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS))
        {
            aiString roughnessFile;
            material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &roughnessFile);
            roughnessPath = directory + '/' + roughnessFile.C_Str();
        }
        // 4. AO map
        if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION))
        {
            aiString aoFile;
            material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &aoFile);
            aoPath = directory + '/' + aoFile.C_Str();
        }
        // 5. Emissive map
        if (material->GetTextureCount(aiTextureType_EMISSIVE))
        {
            aiString emissiveFile;
            material->GetTexture(aiTextureType_EMISSIVE, 0, &emissiveFile);
            emissivePath = directory + '/' + emissiveFile.C_Str();
        }
    }

    bool metalRoughnessIsShared = metallicPath == roughnessPath;
    if (!diffusePath.empty())
        diffuseTexture = LoadTexture(diffusePath, flipVerticalDiffuse);
    if (!normalPath.empty())
        normalTexture = LoadTexture(normalPath, flipVerticalNormal);
    if (!metallicPath.empty())
        metallicTexture = LoadTexture(metallicPath, flipVerticalMetallic, metalRoughnessIsShared, 2);
    if (!roughnessPath.empty())
        roughnessTexture = LoadTexture(roughnessPath, flipVerticalRoughness, metalRoughnessIsShared, 1);
    if (!aoPath.empty())
        aoTexture = LoadTexture(aoPath, flipVerticalAO);
    if (!emissivePath.empty())
        emissiveTexture = LoadTexture(emissivePath, flipVerticalEmissive);

    returnPaths.push_back(diffusePath);
    returnPaths.push_back(normalPath);
    returnPaths.push_back(metallicPath);
    returnPaths.push_back(roughnessPath);
    returnPaths.push_back(aoPath);
    returnPaths.push_back(emissivePath);

    m_LoadedModels[path] = {path, diffusePath, normalPath, metallicPath, roughnessPath, aoPath, emissivePath};

    return returnPaths;
}

void AssetManager::loadDefaultMeshAndTextures()
{
    // Setup default cube
    std::vector<Ref<SubMesh>> subMeshes;
    std::vector<MeshVertex> defaultVertices;

    //Back
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    //Left
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    //Right
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    //Front
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    //Top
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    //Bottom
    defaultVertices.push_back(
        {{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});
    defaultVertices.push_back(
        {{1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}});

    std::vector<uint32_t> defaultIndices;

    const std::string defaultMeshPath = std::string("default");
    const Ref<SubMesh> defaultMesh = CreateRef<SubMesh>(defaultVertices, defaultIndices);
    subMeshes.push_back(defaultMesh);
    const Ref<MeshAsset> defaultMeshAsset = CreateRef<MeshAsset>(IdManager::GetInstance().CreateNewId(), defaultMeshPath, defaultMesh);
    m_LoadedMeshAssets[defaultMeshPath] = defaultMeshAsset;

    //Default "Prototype" Texture
    std::string defaultTexturePath("assets/textures/default.png");
    const Ref<TextureAsset> defaultTextureAsset = LoadTexture(defaultTexturePath, false);
    auto nodeHandle = m_LoadedTextureAssets.extract("assets/textures/default.png");
    nodeHandle.key() = "default";
    m_LoadedTextureAssets.insert(std::move(nodeHandle));
    const Ref<MaterialAsset> defaultMaterial = CreateRef<MaterialAsset>("Default");
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

void AssetManager::processNode(const aiNode* node, const aiScene* scene, std::vector<Ref<SubMesh>>& subMeshes)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        subMeshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, subMeshes);
    }
}

Ref<SubMesh> AssetManager::processMesh(aiMesh* mesh, const aiScene* scene)
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

    // return a SubMesh object created from the extracted mesh data
    return CreateRef<SubMesh>(vertices, indices);
}
