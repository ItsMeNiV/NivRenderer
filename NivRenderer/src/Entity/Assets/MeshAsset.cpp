#include "Entity/Assets/MeshAsset.h"

const std::string& MeshAsset::GetPath()
{
    return m_Path;
}

const std::vector<MeshVertex>& MeshAsset::GetVertices() const
{
    return m_Vertices;
}

const std::vector<uint32_t>& MeshAsset::GetIndices() const
{
    return m_Indices;
}

ordered_json MeshAsset::SerializeObject()
{
    ordered_json mesh = {
        {"Id", GetId()},
        {"InternalPath", m_Path},
    };

    mesh["Vertices"] = json::array();
    uint32_t i = 0;
    for (auto& v : m_Vertices)
    {
        mesh["Vertices"][i] = {
            {"Position",  {{"x", v.Position.x}, {"y", v.Position.y}, {"z", v.Position.z}}},
            {"Normal",    {{"x", v.Normal.x}, {"y", v.Normal.y}, {"z", v.Normal.z}}},
            {"TexCoords", {{"x", v.TexCoords.x}, {"y", v.TexCoords.y}}},
            {"Tangent",   {{"x", v.Tangent.x}, {"y", v.Tangent.y}, {"z", v.Tangent.z}}},
            {"Bitangent", {{"x", v.Bitangent.x}, {"y", v.Bitangent.y}, {"z", v.Bitangent.z}}}
        };
        i++;
    }

    mesh["Indices"] = m_Indices;

    return mesh;
}

void MeshAsset::DeSerializeObject(json jsonObject)
{
    {
        const std::vector<uint32_t> indices = jsonObject["Indices"];
        m_Indices = indices;
    }
    for (json vertex : jsonObject["Vertices"])
    {
        MeshVertex vert;
        vert.Position = {vertex["Position"]["x"], vertex["Position"]["y"], vertex["Position"]["z"]};
        vert.Normal = {vertex["Normal"]["x"], vertex["Normal"]["y"], vertex["Normal"]["z"]};
        vert.TexCoords = {vertex["TexCoords"]["x"], vertex["TexCoords"]["y"]};
        vert.Tangent = {vertex["Tangent"]["x"], vertex["Tangent"]["y"], vertex["Tangent"]["z"]};
        vert.Bitangent = {vertex["Bitangent"]["x"], vertex["Bitangent"]["y"], vertex["Bitangent"]["z"]};

        m_Vertices.push_back(vert);
    }
}
