#include "MeshProxy.h"

MeshProxy::MeshProxy(uint32_t id) :
    Proxy(id), m_VertexArray(UINT32_MAX), m_VertexBuffer(UINT32_MAX), m_IndexBuffer(UINT32_MAX), m_VerticesCount(0), m_IndexCount(0)
{
    glGenVertexArrays(1, &m_VertexArray);
    glGenBuffers(1, &m_VertexBuffer);
    glGenBuffers(1, &m_IndexBuffer);
}

MeshProxy::~MeshProxy()
{
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
    glDeleteVertexArrays(1, &m_VertexArray);
}

void MeshProxy::CreateBuffers(MeshComponent* const mesh)
{
    glBindVertexArray(m_VertexArray);

    const auto& vertices = mesh->GetMeshAsset()->GetVertices();
    const auto& indices = mesh->GetMeshAsset()->GetIndices();

    m_IndexCount = indices.size();
    m_VerticesCount = vertices.size();

    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), vertices.data(), GL_DYNAMIC_DRAW);
    if (m_IndexCount)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCount * sizeof(uint32_t), indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, Position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, Normal)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, TexCoords)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, Tangent)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, Bitangent)));
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void MeshProxy::Bind() const
{
    glBindVertexArray(m_VertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
}

void MeshProxy::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
