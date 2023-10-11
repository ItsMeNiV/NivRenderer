#include "MeshProxy.h"

MeshProxy::MeshProxy(uint32_t id) :
    Proxy(id), m_VertexArray(UINT32_MAX), m_VertexBuffer(UINT32_MAX), m_IndexBuffer(UINT32_MAX), m_VerticesCount(0), m_IndexCount(0)
{
    glCreateVertexArrays(1, &m_VertexArray);
    glCreateBuffers(1, &m_VertexBuffer);
    glCreateBuffers(1, &m_IndexBuffer);
}

MeshProxy::~MeshProxy()
{
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
    glDeleteVertexArrays(1, &m_VertexArray);
}

void MeshProxy::CreateBuffers(const MeshComponent* const mesh)
{
    const auto& vertices = mesh->meshAsset->GetVertices();
    const auto& indices = mesh->meshAsset->GetIndices();

    m_IndexCount = indices.size();
    m_VerticesCount = vertices.size();

    glNamedBufferData(m_VertexBuffer, vertices.size() * sizeof(MeshVertex), vertices.data(), GL_DYNAMIC_DRAW);
    if (m_IndexCount)
        glNamedBufferData(m_IndexBuffer, m_IndexCount * sizeof(uint32_t), indices.data(), GL_DYNAMIC_DRAW);

    glVertexArrayVertexBuffer(m_VertexArray, 0, m_VertexBuffer, 0, sizeof(MeshVertex));
    glVertexArrayElementBuffer(m_VertexArray, m_IndexBuffer);

    glEnableVertexArrayAttrib(m_VertexArray, 0);
    glEnableVertexArrayAttrib(m_VertexArray, 1);
    glEnableVertexArrayAttrib(m_VertexArray, 2);
    glEnableVertexArrayAttrib(m_VertexArray, 3);
    glEnableVertexArrayAttrib(m_VertexArray, 4);

    glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, offsetof(MeshVertex, Position));
    glVertexArrayAttribFormat(m_VertexArray, 1, 3, GL_FLOAT, GL_FALSE, offsetof(MeshVertex, Normal));
    glVertexArrayAttribFormat(m_VertexArray, 2, 2, GL_FLOAT, GL_FALSE, offsetof(MeshVertex, TexCoords));
    glVertexArrayAttribFormat(m_VertexArray, 3, 3, GL_FLOAT, GL_FALSE, offsetof(MeshVertex, Tangent));
    glVertexArrayAttribFormat(m_VertexArray, 4, 3, GL_FLOAT, GL_FALSE, offsetof(MeshVertex, Bitangent));

    glVertexArrayAttribBinding(m_VertexArray, 0, 0);
    glVertexArrayAttribBinding(m_VertexArray, 1, 0);
    glVertexArrayAttribBinding(m_VertexArray, 2, 0);
    glVertexArrayAttribBinding(m_VertexArray, 3, 0);
    glVertexArrayAttribBinding(m_VertexArray, 4, 0);
}

void MeshProxy::Bind() const
{
    glBindVertexArray(m_VertexArray);
}

void MeshProxy::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
