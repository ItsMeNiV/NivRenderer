#include "Entity/Assets/MeshAsset.h"

const std::vector<MeshVertex>& SubMesh::GetVertices() const
{
    return m_Vertices;
}

const std::vector<uint32_t>& SubMesh::GetIndices() const
{
    return m_Indices;
}

const std::vector<Ref<SubMesh>>& MeshAsset::GetSubMeshes() const
{
    return m_SubMeshes;
}

void MeshAsset::AddSubMeshes(const std::vector<Ref<SubMesh>>& meshes)
{
    m_SubMeshes.insert(m_SubMeshes.end(), meshes.begin(), meshes.end());
}
