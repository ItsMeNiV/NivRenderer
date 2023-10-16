#include "NewAssetManager.h"

#include "IdManager.h"

NewAssetManager::NewAssetManager()
{
    m_Importer = CreateScope<Assimp::Importer>();
    loadDefaults();
}

void NewAssetManager::Reset()
{
    m_LoadedShaders.clear();
    m_ShadersByPath.clear();
}

ShaderAsset* NewAssetManager::LoadShader(const std::string& path, ShaderType shaderType)
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

ShaderAsset* NewAssetManager::GetShader(const uint32_t id)
{
    if (m_LoadedShaders.contains(id))
        return m_LoadedShaders[id].get();

    return nullptr;
}

ShaderAsset* NewAssetManager::GetShader(const std::string& path)
{
    if (m_ShadersByPath.contains(path))
        return m_ShadersByPath[path];

    return nullptr;
}

MeshAsset* NewAssetManager::LoadMesh(const std::string& path)
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

MeshAsset* NewAssetManager::GetMesh(uint32_t id)
{
    if (m_LoadedMeshes.contains(id))
        return m_LoadedMeshes[id].get();

    return nullptr;
}

MeshAsset* NewAssetManager::GetMesh(const std::string& path)
{
    if (m_MeshesByPath.contains(path))
        return m_MeshesByPath[path];

    return nullptr;
}

TextureAsset* NewAssetManager::LoadTexture(std::string& path, bool flipVertical, bool loadOnlyOneChannel, uint32_t channelIndex)
{
}

TextureAsset* NewAssetManager::GetTexture(uint32_t id)
{
}

TextureAsset* NewAssetManager::GetTexture(const std::string& path)
{
}

MaterialAsset* NewAssetManager::GetMaterial(const std::string& name)
{
    if (m_MaterialsByName.contains(name))
        return m_MaterialsByName[name];

    return nullptr;
}

MaterialAsset* NewAssetManager::GetMaterial(uint32_t id)
{
    if (m_LoadedMaterials.contains(id))
        return m_LoadedMaterials[id].get();

    return nullptr;
}

MaterialAsset* NewAssetManager::CreateMaterial()
{
    const uint32_t id = IdManager::GetInstance().CreateNewId();
    m_LoadedMaterials[id] = CreateScope<MaterialAsset>(id, "New Material");
    m_MaterialsByName["New Material"] = m_LoadedMaterials[id].get();
    const auto newMaterial = m_LoadedMaterials[id].get();
    newMaterial->diffusePath = "default";
    newMaterial->diffuseTextureAsset = m_TexturesByName["default"];

    return newMaterial;
}

void NewAssetManager::processNode(const aiNode* node, const aiScene* scene, std::vector<SubModel>& subModels,
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

MeshAsset* NewAssetManager::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& path)
{
    auto meshId = IdManager::GetInstance().CreateNewId();
    std::string meshPath = path + '@' + mesh->mName.C_Str();
    m_LoadedMeshes[meshId] = CreateScope<MeshAsset>(meshId, meshPath);
    m_MeshesByPath[meshPath] = m_LoadedMeshes[meshId].get();

    auto& vertices = m_LoadedMeshes[meshId]->vertices;
    auto& indices = m_LoadedMeshes[meshId]->indices;

    vertices.reserve(mesh->mNumVertices);
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

    indices.reserve(mesh->mNumFaces);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // return a MeshAsset created from the extracted mesh data
    return m_LoadedMeshes[meshId].get();
}

void NewAssetManager::processMaterials(const aiScene* scene, SubModel& subModel, const std::string& path,
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
        *materialAsset->GetMetallicTextureAsset() =
            LoadTexture(metallicPath, materialAsset->GetFlipMetallicTexture(), metalRoughnessIsShared, 2);
    if (!roughnessPath.empty())
        *materialAsset->GetRoughnessTextureAsset() =
            LoadTexture(roughnessPath, materialAsset->GetFlipRoughnessTexture(), metalRoughnessIsShared, 1);
    if (!aoPath.empty())
        *materialAsset->GetAOTextureAsset() = LoadTexture(aoPath, materialAsset->GetFlipAOTexture());
    if (!emissivePath.empty())
        *materialAsset->GetEmissiveTextureAsset() = LoadTexture(emissivePath, materialAsset->GetFlipEmissiveTexture());

    subModel.material = materialAsset;
}