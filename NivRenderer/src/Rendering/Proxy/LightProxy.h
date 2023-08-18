#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"

class LightProxy : public Proxy
{
public:
    LightProxy(uint32_t id) : Proxy(id), m_ModelMatrix(1.0f)
    {
        if (m_VertexArray == UINT32_MAX)
        {
            glGenVertexArrays(1, &m_VertexArray);
            glGenBuffers(1, &m_VertexBuffer);

            glBindVertexArray(m_VertexArray);

            std::vector<MeshVertex> vertices;

            for (auto& m : AssetManager::GetInstance().LoadMesh("default")->GetSubMeshes())
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
            }
            m_VerticesCount = vertices.size();

            glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), vertices.data(), GL_DYNAMIC_DRAW);

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

            glBindVertexArray(0);
        }
    }

    const glm::vec3& GetLightColor() const { return m_LightColor; }
    glm::mat4& GetModelMatrix() { return m_ModelMatrix; }
    static uint32_t GetVerticesCount() { return m_VerticesCount; }
    static void Bind()
    {
        glBindVertexArray(m_VertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    }

    void SetLightColor(const glm::vec3& color) { m_LightColor = color; }

protected:
    glm::vec3 m_LightColor;
    glm::mat4 m_ModelMatrix;

private:
    // TODO: Abstract
    inline static uint32_t m_VertexArray = UINT32_MAX;
    inline static uint32_t m_VertexBuffer = UINT32_MAX;
    inline static uint32_t m_VerticesCount;
};

class DirectionalLightProxy : public LightProxy
{
public:
    DirectionalLightProxy(uint32_t id) : LightProxy(id) {}

    void UpdateData(glm::vec3 lightColor, glm::vec3 direction)
    {
        m_LightColor = lightColor;
        m_LightDirection = direction;
    }

    const glm::vec3& GetLightDirection() const { return m_LightDirection; }

    void SetLightDirection(const glm::vec3& direction) { m_LightDirection = direction; }

private:
    glm::vec3 m_LightDirection;
};

class PointLightProxy : public LightProxy
{
public:
    PointLightProxy(uint32_t id) : LightProxy(id) {}

    void UpdateData(glm::vec3 lightColor, glm::vec3 position, uint32_t strength)
    {
        m_LightColor = lightColor;
        m_LightPosition = position;
        m_LightStrength = strength;
        m_ModelMatrix = glm::mat4(1.0f);
        m_ModelMatrix = glm::translate(m_ModelMatrix, position);
        m_ModelMatrix = glm::scale(m_ModelMatrix, {0.1f, 0.1f, 0.1f});
    }

    const glm::vec3& GetLightPosition() const { return m_LightPosition; }
    const uint32_t& GetLightStrength() const { return m_LightStrength; }

    void SetLightPosition(const glm::vec3& position) { m_LightPosition = position; }
    void SetLightStrength(const uint32_t& strength) { m_LightStrength = strength; }

private:
    glm::vec3 m_LightPosition;
    uint32_t m_LightStrength;
};