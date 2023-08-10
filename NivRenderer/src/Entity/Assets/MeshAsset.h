#pragma once
#include "Base.h"

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

class MeshAsset
{
public:
    MeshAsset() = default;
    MeshAsset(const Ref<SubMesh>& subMesh) { m_SubMeshes.push_back(subMesh); }

    const std::vector<Ref<SubMesh>>& GetSubMeshes() const;
    void AddSubMeshes(const std::vector<Ref<SubMesh>>& meshes);

private:
    std::vector<Ref<SubMesh>> m_SubMeshes;

};
