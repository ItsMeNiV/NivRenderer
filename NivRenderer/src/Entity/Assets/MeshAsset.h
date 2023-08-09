#pragma once

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
    SubMesh(std::vector<MeshVertex> vertices, std::vector<uint32_t> indices) :
        m_Vertices(vertices), m_Indices(indices)
    {
    }

    const std::vector<MeshVertex> &GetVertices() const { return m_Vertices; }
    const std::vector<uint32_t> &GetIndices() const { return m_Indices; }

private:
    std::vector<MeshVertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
};

class MeshAsset
{
public:


private:

};