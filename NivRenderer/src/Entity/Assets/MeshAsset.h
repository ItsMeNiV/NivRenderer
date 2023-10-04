#pragma once
#include "Base.h"
#include "Entity/PropertyType.h"
#include "json.hpp"

struct MeshVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class MeshAsset
{
public:
    MeshAsset(const uint32_t id, const std::string& path, const std::vector<MeshVertex>& vertices, const std::vector<uint32_t>& indices) :
        m_Id(id), m_Path(path), m_Vertices(vertices), m_Indices(indices)
    {}
    MeshAsset(const uint32_t id, const std::string& path) : m_Id(id), m_Path(path)
    {}

    uint32_t GetId() const;
    const std::string& GetPath();
    const std::vector<MeshVertex>& GetVertices() const;
    const std::vector<uint32_t>& GetIndices() const;

    std::vector<std::pair<std::string, Property>> GetAssetProperties()
    {
        std::vector<std::pair<std::string, Property>> returnVector;
        return returnVector;
    }

    nlohmann::ordered_json SerializeObject();
    void DeSerializeObject(nlohmann::json jsonObject);

private:
    uint32_t m_Id;
    std::string m_Path;
    std::vector<MeshVertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
};
