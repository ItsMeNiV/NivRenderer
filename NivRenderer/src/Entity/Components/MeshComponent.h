#pragma once
#include "Base.h"
#include "Entity/Component.h"
#include "Entity/Assets/AssetManager.h"
//TODO Needs to be moved into it's own assetloader component
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "stb_image.h"

struct MeshTexture
{
	uint32_t id;
	std::string type;
    std::string path;
};

class Mesh
{
public:
    Mesh(std::vector<MeshVertex> vertices, std::vector<uint32_t> indices, std::vector<MeshTexture> textures)
        : m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
    {}

    const std::vector<MeshVertex>& GetVertices() const { return m_Vertices; }
    const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
    const std::vector<MeshTexture>& GetTextures() const { return m_Textures; }

private:
	std::vector<MeshVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	std::vector<MeshTexture> m_Textures;
};

class MeshComponent : public Component
{
public:
	MeshComponent()
		: Component("MeshComponent"), m_Path("")
	{
		//Setup default cube
        std::vector<MeshVertex> defaultVertices;
        defaultVertices.push_back({ { -1.0f,1.0f,-1.0f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,-1.0f,-1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,-1.0f,-1.0f},  {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,-1.0f,-1.0f},  {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,1.0f,-1.0f},   {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,1.0f,-1.0f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        defaultVertices.push_back({ { -1.0f,-1.0f,1.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,-1.0f,-1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,1.0f,-1.0f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,1.0f,-1.0f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,1.0f,1.0f},   {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,-1.0f,1.0f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        defaultVertices.push_back({ { 1.0f,-1.0f,-1.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,-1.0f,1.0f},   {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,1.0f,1.0f},    {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,1.0f,1.0f},    {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,1.0f,-1.0f},   {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,-1.0f,-1.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        defaultVertices.push_back({ { -1.0f,-1.0f,1.0f},  {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,1.0f,1.0f},   {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,1.0f,1.0f},    {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,1.0f,1.0f},    {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,-1.0f,1.0f},   {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,-1.0f,1.0f},  {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        defaultVertices.push_back({ { -1.0f,1.0f,-1.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,1.0f,-1.0f},   {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,1.0f,1.0f},    {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,1.0f,1.0f},    {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,1.0f,1.0f},   {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,1.0f,-1.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        defaultVertices.push_back({ { -1.0f,-1.0f,-1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,-1.0f,1.0f},  {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,-1.0f,-1.0f},  {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,-1.0f,-1.0f},  {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { -1.0f,-1.0f,1.0f},  {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });
        defaultVertices.push_back({ { 1.0f,-1.0f,1.0f},   {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } });

        std::vector<uint32_t> defaultIndices;
        std::vector<MeshTexture> defaultTextures;

        MeshTexture defaultTexture;
        defaultTexture.id = TextureFromFile("default.png", "assets/textures");
        defaultTexture.type = "texture_diffuse";
        defaultTexture.path = "assets/textures/default.png";
        defaultTextures.push_back(defaultTexture);
        textures_loaded.push_back(defaultTexture);

        Ref<Mesh> defaultMesh = CreateRef<Mesh>(defaultVertices, defaultIndices, defaultTextures);
        m_SubMeshes.push_back(defaultMesh);
	}

	~MeshComponent()
	{}

    const std::vector<Ref<Mesh>>& GetSubMeshes() const { return m_SubMeshes; }

	virtual std::unordered_map<std::string, ComponentProperty> GetComponentProperties()
	{
		std::unordered_map<std::string, ComponentProperty> returnMap;

		returnMap["Path"] = { PropertyType::PATH, &m_Path, [this](){
			reloadMesh();
		}};

		return returnMap;
	}

private:
	std::string m_Path;
    std::string m_Directory;
	std::vector<Ref<Mesh>> m_SubMeshes;
    //TODO Needs to be moved into it's own assetloader component
    std::vector<MeshTexture> textures_loaded;

    void reloadMesh()
    {
        AssetManager::GetInstance().LoadMesh(m_Path);
        //TODO: This logic should be moved to a separate AssetHandler. Loading Models and Textures that are needed, etc
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(m_Path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        m_Directory = m_Path.substr(0, m_Path.find_last_of('/'));
        
        m_SubMeshes.clear();
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_SubMeshes.push_back(processMesh(mesh, scene));
        }
        // then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<MeshTexture> textures;

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

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // 1. diffuse maps
        std::vector<MeshTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<MeshTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<MeshTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<MeshTexture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a mesh object created from the extracted mesh data
        return CreateRef<Mesh>(vertices, indices, textures);
    }

    std::vector<MeshTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<MeshTexture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                MeshTexture texture;
                texture.id = TextureFromFile(str.C_Str(), m_Directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    unsigned int TextureFromFile(const char* path, const std::string& directory)
    {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
};