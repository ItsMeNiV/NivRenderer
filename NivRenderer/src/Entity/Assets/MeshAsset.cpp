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
