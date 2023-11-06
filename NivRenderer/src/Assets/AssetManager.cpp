#include "AssetManager.h"

#include "IdManager.h"
#include "stb_image.h"
#include "stb_image_resize.h"

AssetManager::AssetManager()
{
    m_Importer = CreateScope<Assimp::Importer>();
    loadDefaults();
}

void AssetManager::Reset()
{
    m_LoadedShaders.clear();
    m_ShadersByPath.clear();
}

ShaderAsset* AssetManager::LoadShader(const std::string& path, ShaderType shaderType)
{
    if (auto shader = new Shader(path.c_str(), shaderType))
    {
        auto shaderId = IdManager::GetInstance().CreateNewId();
        m_LoadedShaders[shaderId] = CreateScope<ShaderAsset>(shaderId, shader);
        m_ShadersByPath[path] = m_LoadedShaders[shaderId].get();
        return m_ShadersByPath[path];
    }

    return nullptr;
}

ShaderAsset* AssetManager::GetShader(const uint32_t id)
{
    if (m_LoadedShaders.contains(id))
        return m_LoadedShaders[id].get();

    return nullptr;
}

ShaderAsset* AssetManager::GetShader(const std::string& path)
{
    if (m_ShadersByPath.contains(path))
        return m_ShadersByPath[path];

    return nullptr;
}

MeshAsset* AssetManager::LoadMesh(const std::string& path)
{
    const aiScene* scene = m_Importer->ReadFile(path, MESH_IMPORT_POSTPROCESS_FLAGS);

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

MeshAsset* AssetManager::GetMesh(uint32_t id)
{
    if (m_LoadedMeshes.contains(id))
        return m_LoadedMeshes[id].get();

    return nullptr;
}

MeshAsset* AssetManager::GetMesh(const std::string& path)
{
    if (m_MeshesByPath.contains(path))
        return m_MeshesByPath[path];

    return nullptr;
}

TextureAsset* AssetManager::LoadTexture(std::string& path, bool flipVertical, bool loadOnlyOneChannel, uint32_t channelIndex)
{
    // Replace default paths
    if (s_DefaultTexturesMap.contains(path))
        path = s_DefaultTexturesMap[path];

    if (loadOnlyOneChannel)
    {
        std::string fileName = path.substr(0, path.find_last_of('.'));
        const std::string fileEnding = path.substr(path.find_last_of('.'), path.size());
        fileName += "_@" + std::to_string(channelIndex);
        path = fileName + fileEnding;
    }

    std::string pathToUse = path;
    if (path.find_last_of('@') != std::string::npos)
    {
        loadOnlyOneChannel = true;
        channelIndex = std::stoi(path.substr(path.find_last_of('@') + 1, path.size()));
        pathToUse = path.substr(0, path.find_last_of('@') - 1) + path.substr(path.find_last_of('@') + 2, path.size());
    }

    if (auto textureAsset = m_TexturesByPath[path])
    {
        textureAsset->flipVertical = flipVertical;
        stbi_set_flip_vertically_on_load(flipVertical);

        delete[] textureAsset->textureData;
        importTexture(textureAsset);

        return textureAsset;
    }
    else
    {
        const auto textureId = IdManager::GetInstance().CreateNewId();
        m_LoadedTextures[textureId] = CreateScope<TextureAsset>(textureId, pathToUse, flipVertical, loadOnlyOneChannel, channelIndex);
        m_TexturesByPath[path] = m_LoadedTextures[textureId].get();
       textureAsset = m_LoadedTextures[textureId].get();

        stbi_set_flip_vertically_on_load(textureAsset->flipVertical);

        importTexture(textureAsset);

        return textureAsset;
    }
}

TextureAsset* AssetManager::GetTexture(uint32_t id)
{
    if (m_LoadedTextures.contains(id))
        return m_LoadedTextures[id].get();

    return nullptr;
}

TextureAsset* AssetManager::GetTexture(const std::string& path)
{
    // Replace default paths
    std::string pathKey = path;
    if (s_DefaultTexturesMap.contains(path))
        pathKey = s_DefaultTexturesMap[path];

    if (m_TexturesByPath.contains(pathKey))
        return m_TexturesByPath[pathKey];

    return nullptr;
}

std::vector<uint32_t> AssetManager::GetTextureIds(bool includeDefault) const
{
    std::vector<uint32_t> returnVector;
    for (auto& it : m_LoadedTextures)
    {
        const bool isDefaultPath = it.second->path == s_DefaultTexturesMap["default"] ||
            it.second->path == s_DefaultTexturesMap["black"] || it.second->path == s_DefaultTexturesMap["white"];
        if (!includeDefault && isDefaultPath)
            continue;

        returnVector.push_back(it.first);
    }

    return returnVector;
}

MaterialAsset* AssetManager::GetMaterial(const std::string& name)
{
    if (m_MaterialsByName.contains(name))
        return m_MaterialsByName[name];

    return nullptr;
}

MaterialAsset* AssetManager::GetMaterial(uint32_t id)
{
    if (m_LoadedMaterials.contains(id))
        return m_LoadedMaterials[id].get();

    return nullptr;
}

MaterialAsset* AssetManager::CreateMaterial()
{
    const uint32_t id = IdManager::GetInstance().CreateNewId();
    const std::string materialName = "New Material (" + std::to_string(id) + ")";
    m_LoadedMaterials[id] = CreateScope<MaterialAsset>(id, materialName);
    m_MaterialsByName[materialName] = m_LoadedMaterials[id].get();
    const auto newMaterial = m_LoadedMaterials[id].get();
    newMaterial->diffusePath = "default";
    newMaterial->diffuseTextureAsset = m_TexturesByPath["default"];

    return newMaterial;
}

void AssetManager::RemoveMaterial(uint32_t id)
{
    const auto material = m_LoadedMaterials[id].get();
    m_MaterialsByName.erase(material->name);
    m_LoadedMaterials.erase(id);
}

std::vector<uint32_t> AssetManager::GetMaterialIds(bool includeDefault) const
{
    std::vector<uint32_t> returnVector;
    for (auto& it : m_LoadedMaterials)
    {
        if (!includeDefault && it.second->name == "Default")
            continue;

        returnVector.push_back(it.first);
    }

    return returnVector;
}

Model* AssetManager::LoadModel(const std::string& path)
{
    const aiScene* scene = m_Importer->ReadFile(
        path, MESH_IMPORT_POSTPROCESS_FLAGS);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_DEBUG(std::string("ERROR::ASSIMP::") + m_Importer->GetErrorString());

        return nullptr;
    }

    const auto model = new Model();
    processNode(scene->mRootNode, scene, model->subModels, path);
    model->name = path.substr(path.find_last_of('/') + 1, path.find_last_of('.'));

    return model;
}

void AssetManager::loadDefaults()
{
    // Setup default cube
    std::vector<MeshVertex> defaultVertices;

    // Back
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

    // Left
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

    // Right
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

    // Front
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

    // Top
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

    // Bottom
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

    m_LoadedMeshes[0] = CreateScope<MeshAsset>(0, "default", defaultVertices, defaultIndices);
    m_MeshesByPath["default"] = m_LoadedMeshes[0].get();

    // Default "Prototype" Texture and Material
    m_LoadedTextures[1] = CreateScope<TextureAsset>(1, "assets/textures/default.png", false, false, 0);
    m_TexturesByPath["assets/textures/default.png"] = m_LoadedTextures[1].get();
    importTexture(m_LoadedTextures[1].get());
    m_LoadedMaterials[2] = CreateScope<MaterialAsset>(2, "Default");
    m_MaterialsByName["Default"] = m_LoadedMaterials[2].get();
    const auto defaultMaterial = m_LoadedMaterials[2].get();
    defaultMaterial->diffusePath = "assets/textures/default.png";
    defaultMaterial->diffuseTextureAsset = m_LoadedTextures[1].get();

    // White 1x1 Texture
    m_LoadedTextures[3] = CreateScope<TextureAsset>(3, "assets/textures/default_white.png", false, false, 0);
    m_TexturesByPath["assets/textures/default_white.png"] = m_LoadedTextures[3].get();
    importTexture(m_LoadedTextures[3].get());

    // Black 1x1 Texture
    m_LoadedTextures[4] = CreateScope<TextureAsset>(4, "assets/textures/default_black.png", false, false, 0);
    m_TexturesByPath["assets/textures/default_black.png"] = m_LoadedTextures[4].get();
    importTexture(m_LoadedTextures[4].get());
}

void AssetManager::importTexture(TextureAsset* textureAsset)
{
    const std::string pathToUse = textureAsset->path;
    unsigned char* loadedData = stbi_load(pathToUse.c_str(), &textureAsset->width, &textureAsset->height,
                                          &textureAsset->nrComponents, 0);

    unsigned char* resizedData = nullptr;
    if (textureAsset->width > 4000 && textureAsset->height > 4000)
    {
        resizedData = new unsigned char[1920 * 1080 * textureAsset->nrComponents];
        stbir_resize_uint8(loadedData, textureAsset->width, textureAsset->height, 0, resizedData, 1920, 1080, 0, textureAsset->nrComponents);
        textureAsset->width = 1920;
        textureAsset->height = 1080;
        stbi_image_free(loadedData);
    }

    if (textureAsset->loadOnlyOneChannel)
    {
        const size_t dataCount = textureAsset->width * textureAsset->height;
        textureAsset->textureData = new unsigned char[dataCount];

        for (int32_t y = 0; y < textureAsset->height; y++)
        {
            for (int32_t x = 0; x < textureAsset->width; x++)
            {
                const unsigned char* pixelOffset =
                    textureAsset->nrComponents * (y * textureAsset->width + x) +
                    (resizedData ? resizedData : loadedData);
                textureAsset->textureData[y * textureAsset->width + x] = pixelOffset[textureAsset->channelIndex];
            }
        }
        textureAsset->nrComponents = 1;

        stbi_image_free(resizedData ? resizedData : loadedData);
    }
    else
    {
        const size_t dataCount = textureAsset->width * textureAsset->height * textureAsset->nrComponents;
        const size_t dataSize = sizeof(unsigned char) * dataCount;
        textureAsset->textureData = new unsigned char[dataCount];
        memcpy(textureAsset->textureData, resizedData ? resizedData : loadedData, dataSize);
        stbi_image_free(resizedData ? resizedData : loadedData);
    }
}

void AssetManager::processNode(const aiNode* node, const aiScene* scene, std::vector<SubModel>& subModels,
                                  const std::string& path)
{
    SubModel subModelNode;
    subModelNode.name = node->mName.C_Str();
    aiMatrix4x4 modelMat = node->mTransformation;
    subModelNode.modelMatrix = {modelMat.a1, modelMat.b1, modelMat.c1, modelMat.d1, modelMat.a2, modelMat.b2,
                                modelMat.c2, modelMat.d2, modelMat.a3, modelMat.b3, modelMat.c3, modelMat.d3,
                                modelMat.a4, modelMat.b4, modelMat.c4, modelMat.d4};

    subModelNode.subModels.reserve(node->mNumMeshes);
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
    auto meshId = IdManager::GetInstance().CreateNewId();
    std::string meshPath = path + '@' + mesh->mName.C_Str();
    m_LoadedMeshes[meshId] = CreateScope<MeshAsset>(meshId, meshPath, mesh->mNumVertices, mesh->mNumFaces);
    m_MeshesByPath[meshPath] = m_LoadedMeshes[meshId].get();

    auto& vertices = m_LoadedMeshes[meshId]->vertices;
    auto& indices = m_LoadedMeshes[meshId]->indices;

    vertices.resize(mesh->mNumVertices);
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

        vertices[i] = vertex;
    }

    indices.resize(3 * mesh->mNumFaces);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices[(i*3) + j] = face.mIndices[j];
    }

    // return a MeshAsset created from the extracted mesh data
    return m_LoadedMeshes[meshId].get();
}

void AssetManager::processMaterials(const aiScene* scene, SubModel& subModel, const std::string& path,
                                       const uint32_t materialIndex)
{
    const std::string directory = path.substr(0, path.find_last_of('/'));
    const std::string materialName = path + '@' + std::to_string(materialIndex);
    MaterialAsset* materialAsset = GetMaterial(materialName);
    if (materialAsset)
    {
        subModel.material = materialAsset;
        return;
    }

    const auto materialAssetId = IdManager::GetInstance().CreateNewId();
    m_LoadedMaterials[materialAssetId] = CreateScope<MaterialAsset>(materialAssetId, materialName);
    m_MaterialsByName[materialName] = m_LoadedMaterials[materialAssetId].get();
    materialAsset = m_LoadedMaterials[materialAssetId].get();

    auto& diffusePath = materialAsset->diffusePath;
    auto& normalPath = materialAsset->normalPath;
    auto& metallicPath = materialAsset->metallicPath;
    auto& roughnessPath = materialAsset->roughnessPath;
    auto& aoPath = materialAsset->aoPath;
    auto& emissivePath = materialAsset->emissivePath;

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
        materialAsset->diffuseTextureAsset = LoadTexture(diffusePath, materialAsset->flipDiffuseTexture);

    if (!normalPath.empty())
        materialAsset->normalTextureAsset = LoadTexture(normalPath, materialAsset->flipNormalTexture);

    if (!metallicPath.empty())
        materialAsset->metallicTextureAsset = LoadTexture(metallicPath, materialAsset->flipMetallicTexture, metalRoughnessIsShared, 2);

    if (!roughnessPath.empty())
        materialAsset->roughnessTextureAsset = LoadTexture(roughnessPath, materialAsset->flipRoughnessTexture, metalRoughnessIsShared, 1);

    if (!aoPath.empty())
        materialAsset->aoTextureAsset = LoadTexture(aoPath, materialAsset->flipAOTexture);

    if (!emissivePath.empty())
        materialAsset->emissiveTextureAsset = LoadTexture(emissivePath, materialAsset->flipEmissiveTexture);

    subModel.material = materialAsset;
}