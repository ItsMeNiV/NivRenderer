#pragma once
#include "Base.h"
#include "Asset.h"
#include "Entity/PropertyType.h"

struct MeshVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class SubMesh
{
public:
    SubMesh(const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices) :
        m_Vertices(vertices), m_Indices(indices)
    {
    }

    const std::vector<MeshVertex>& GetVertices() const;
    const std::vector<uint32_t>& GetIndices() const;

private:
    std::vector<MeshVertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
};

class MeshAsset : public Asset
{
public:
    MeshAsset(const uint32_t id, const std::string& path) : Asset(id), m_Path(path) {}
    MeshAsset(const uint32_t id, const std::string& path, const Ref<SubMesh>& subMesh) : Asset(id), m_Path(path)
    {
        m_SubMeshes.push_back(subMesh);
    }

    const std::vector<Ref<SubMesh>>& GetSubMeshes() const;
    void AddSubMeshes(const std::vector<Ref<SubMesh>>& meshes);

    const std::string& GetPath();

    std::vector<std::pair<std::string, NivRenderer::Property>> GetAssetProperties() override
    {
        std::vector<std::pair<std::string, NivRenderer::Property>> returnVector;
        return returnVector;
    }

private:
    std::vector<Ref<SubMesh>> m_SubMeshes;
    std::string m_Path;

};
