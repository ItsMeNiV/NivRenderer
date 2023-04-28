#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"
#include "Entity/Components/MeshComponent.h"

class SceneObjectProxy : public Proxy
{
public:
    SceneObjectProxy(uint32_t id) : Proxy(id)
    {
        glGenVertexArrays(1, &m_VertexArray);
        glGenBuffers(1, &m_VertexBuffer);
        glGenBuffers(1, &m_IndexBuffer);
    }

    void SetTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
    {
        m_ModelMatrix = glm::mat4(1.0f);
        m_ModelMatrix = glm::translate(m_ModelMatrix, position);
        //m_ModelMatrix = glm::rotate(m_ModelMatrix, position); TODO
        m_ModelMatrix = glm::scale(m_ModelMatrix, scale);
    }

    void SetMesh(Ref<MeshComponent> mesh)
    {
        glBindVertexArray(m_VertexArray);

        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> indices;

        for (auto& m : mesh->GetSubMeshes())
        {
            uint32_t vertCount = vertices.size();
            auto verts = m->GetVertices();
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
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), &vertices[0], GL_DYNAMIC_DRAW);
        if(m_IndexCount)
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCount * sizeof(uint32_t), &indices[0], GL_DYNAMIC_DRAW);

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

    void Bind()
    {
        glBindVertexArray(m_VertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    }

    void Unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    const uint32_t GetIndexCount() const { return m_IndexCount; }
    const uint32_t GetVerticesCount() const { return m_VerticesCount; }
    glm::mat4& GetModelMatrix() { return m_ModelMatrix; }

private:
    glm::mat4 m_ModelMatrix;

    //TODO: Abstract
    uint32_t m_VertexArray, m_VertexBuffer, m_IndexBuffer;
    uint32_t m_IndexCount, m_VerticesCount;
};