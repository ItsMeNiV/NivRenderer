#include "AssetManager.h"


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

    Ref<MeshAsset> meshAsset = CreateRef<MeshAsset>();
    meshAsset->AddSubMeshes(subMeshes);
    m_LoadedMeshAssets[path] = meshAsset;

    return meshAsset;
}

Ref<TextureAsset> AssetManager::LoadTexture(std::string const& path, bool flipVertical)
{
    const bool textureExists = m_LoadedTextureAssets.contains(path);
    if (textureExists && m_LoadedTextureAssets[path]->GetFlipVertical() == flipVertical)
        return m_LoadedTextureAssets[path];

    std::string pathToUse = textureExists ? m_LoadedTextureAssets[path]->GetPath() : path;

    Ref<TextureAsset> textureAsset = CreateRef<TextureAsset>(pathToUse, flipVertical);

    stbi_set_flip_vertically_on_load(textureAsset->GetFlipVertical());
    textureAsset->SetTextureData(stbi_load(pathToUse.c_str(), textureAsset->GetWidth(), textureAsset->GetHeight(), textureAsset->GetNrComponents(), 0));
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

std::vector<std::string> AssetManager::LoadMeshAndTextures(std::string& path, Ref<MeshAsset>& mesh, Ref<TextureAsset>& diffuseTexture,
                                       Ref<TextureAsset>& specularTexture, Ref<TextureAsset>& normalTexture,
                                       bool flipVerticalDiffuse, bool flipVerticalSpecular, bool flipVerticalNormal)
{
    std::vector<std::string> returnPaths;
    std::string directory = path.substr(0, path.find_last_of('/'));

    const aiScene* scene = m_Importer->ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_DEBUG(std::string("ERROR::ASSIMP::") + m_Importer->GetErrorString());
        return returnPaths;
    }

    if (m_LoadedMeshAssets.contains(path))
    {
        mesh = m_LoadedMeshAssets[path];
        returnPaths.push_back(path);
    }
    else
    {
        Ref<MeshAsset> newMesh = CreateRef<MeshAsset>();
        std::vector<Ref<SubMesh>> subMeshes;
        processNode(scene->mRootNode, scene, subMeshes);

        newMesh->AddSubMeshes(subMeshes);
        m_LoadedMeshAssets[path] = newMesh;
        mesh = newMesh;
        returnPaths.push_back(path);
    }

    //Load Texture paths. We assume one material per model and one texture of each type
    aiMaterial* material = scene->mMaterials[0];
    std::string diffusePath;
    std::string specularPath;
    std::string normalPath;

    // 1. diffuse map
    if (material->GetTextureCount(aiTextureType_DIFFUSE))
    {
        aiString diffuseFile;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseFile);
        diffusePath = directory + '/' + diffuseFile.C_Str();
    }
    // 2. specular map
    if (material->GetTextureCount(aiTextureType_SPECULAR))
    {
        aiString specularFile;
        material->GetTexture(aiTextureType_SPECULAR, 0, &specularFile);
        specularPath = directory + '/' + specularFile.C_Str();
    }
    // 3. normal maps
    if (material->GetTextureCount(aiTextureType_NORMALS))
    {
        aiString normalFile;
        material->GetTexture(aiTextureType_NORMALS, 0, &normalFile);
        normalPath = directory + '/' + normalFile.C_Str();
    }

    returnPaths.push_back(diffusePath);
    returnPaths.push_back(specularPath);
    returnPaths.push_back(normalPath);

    if (!diffusePath.empty())
        diffuseTexture = LoadTexture(diffusePath, flipVerticalDiffuse);
    if (!specularPath.empty())
        specularTexture = LoadTexture(specularPath, flipVerticalDiffuse);
    if (!normalPath.empty())
        normalTexture = LoadTexture(normalPath, flipVerticalDiffuse);

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

    const Ref<SubMesh> defaultMesh = CreateRef<SubMesh>(defaultVertices, defaultIndices);
    subMeshes.push_back(defaultMesh);
    const Ref<MeshAsset> defaultMeshAsset = CreateRef<MeshAsset>(defaultMesh);
    m_LoadedMeshAssets["default"] = defaultMeshAsset;

    const std::string defaultTexturePath("assets/textures/default.png");
    const Ref<TextureAsset> defaultTexture = LoadTexture(defaultTexturePath, false);
    auto nodeHandle = m_LoadedTextureAssets.extract("assets/textures/default.png");
    nodeHandle.key() = "default";
    m_LoadedTextureAssets.insert(std::move(nodeHandle));
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
