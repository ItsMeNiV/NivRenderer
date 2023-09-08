#pragma once
#include "Base.h"
#include "Entity/Asset.h"
#include "Entity/PropertyType.h"

struct MeshVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class MeshAsset : public Asset
{
public:
    MeshAsset(const uint32_t id, const std::string& path, const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices) :
        Asset(id), m_Path(path), m_Vertices(vertices), m_Indices(indices)
    {}

    const std::string& GetPath();
    const std::vector<MeshVertex>& GetVertices() const;
    const std::vector<uint32_t>& GetIndices() const;

    std::vector<std::pair<std::string, Property>> GetAssetProperties() override
    {
        std::vector<std::pair<std::string, Property>> returnVector;
        return returnVector;
    }

private:
    std::string m_Path;
    std::vector<MeshVertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
};
