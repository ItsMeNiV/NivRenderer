#include "Rendering/Proxy/SceneObjectProxy.h"

SceneObjectProxy::SceneObjectProxy(uint32_t id): Proxy(id), m_DiffuseTexture(UINT32_MAX), m_SpecularTexture(UINT32_MAX), m_NormalTexture(UINT32_MAX)
{
    glGenVertexArrays(1, &m_VertexArray);
    glGenBuffers(1, &m_VertexBuffer);
    glGenBuffers(1, &m_IndexBuffer);
}

SceneObjectProxy::~SceneObjectProxy()
{
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
    glDeleteVertexArrays(1, &m_VertexArray);
}

void SceneObjectProxy::SetTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    m_ModelMatrix = glm::mat4(1.0f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, position);
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, scale);
}

void SceneObjectProxy::SetMesh(const Ref<MeshComponent>& mesh)
{
    glBindVertexArray(m_VertexArray);

    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

    for (auto& m : mesh->GetMeshAsset()->GetSubMeshes())
    {
        uint32_t vertCount = vertices.size();
        auto& verts = m->GetVertices();
        auto inds = m->GetIndices();
        if (vertCount > 0)
        {
            for (auto& i : inds)
                i += vertCount;
        }
        vertices.insert(vertices.end(), verts.begin(), verts.end());
        indices.insert(indices.end(), inds.begin(), inds.end());
    }
    m_IndexCount = indices.size();
    m_VerticesCount = vertices.size();

    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), vertices.data(), GL_DYNAMIC_DRAW);
    if(m_IndexCount)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCount * sizeof(uint32_t), indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, TexCoords));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Tangent));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Bitangent));
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void SceneObjectProxy::SetMaterial(const Ref<MaterialComponent>& material)
{
    if (m_DiffuseTexture != UINT32_MAX)
        glDeleteTextures(1, &m_DiffuseTexture);
    if (m_SpecularTexture != UINT32_MAX)
        glDeleteTextures(1, &m_SpecularTexture);
    if (m_NormalTexture != UINT32_MAX)
        glDeleteTextures(1, &m_NormalTexture);

    if (!material->GetDiffusePath().empty())
        createTextureFromAsset(material->GetDiffuseTextureAsset(), &m_DiffuseTexture);
    if (!material->GetMetallicPath().empty())
        createTextureFromAsset(material->GetMetallicTextureAsset(), &m_SpecularTexture);
    if (!material->GetNormalPath().empty())
        createTextureFromAsset(material->GetNormalTextureAsset(), &m_NormalTexture);
}

void SceneObjectProxy::Bind() const
{
    glBindVertexArray(m_VertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
}

void SceneObjectProxy::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool SceneObjectProxy::HasDiffuseTexture() const
{
    return m_DiffuseTexture != UINT32_MAX;
}

bool SceneObjectProxy::HasSpecularTexture() const
{
    return m_SpecularTexture != UINT32_MAX;
}

bool SceneObjectProxy::HasNormalTexture() const
{
    return m_NormalTexture != UINT32_MAX;
}

void SceneObjectProxy::BindDiffuseTexture(const int32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_DiffuseTexture);
}

void SceneObjectProxy::BindSpecularTexture(const int32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_SpecularTexture);
}

void SceneObjectProxy::BindNormalTexture(const int32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_NormalTexture);
}

void SceneObjectProxy::createTextureFromAsset(const Ref<TextureAsset>& textureAsset, uint32_t* textureId)
{
    glGenTextures(1, textureId);
    GLenum format;
    switch (*textureAsset->GetNrComponents())
    {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
    default:
        format = GL_RGBA;
        break;
    }
    glBindTexture(GL_TEXTURE_2D, *textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, *textureAsset->GetWidth(), *textureAsset->GetHeight(), 0, format,
                 GL_UNSIGNED_BYTE, textureAsset->GetTextureData());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}
