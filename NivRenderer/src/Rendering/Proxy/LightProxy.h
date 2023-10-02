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
            glCreateBuffers(1, &m_VertexBuffer);

            const auto& vertices = AssetManager::GetInstance().LoadMesh("default")->GetVertices();
            m_VerticesCount = vertices.size();

            glNamedBufferData(m_VertexBuffer, m_VerticesCount * sizeof(MeshVertex), vertices.data(), GL_STATIC_DRAW);

            glCreateVertexArrays(1, &m_VertexArray);
            glVertexArrayVertexBuffer(m_VertexArray, 0, m_VertexBuffer, 0, sizeof(MeshVertex));

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
    }

    const glm::vec3& GetLightColor() const { return m_LightColor; }
    glm::mat4& GetModelMatrix() { return m_ModelMatrix; }
    static uint32_t GetVerticesCount() { return m_VerticesCount; }
    static void Bind()
    {
        glBindVertexArray(m_VertexArray);
    }
    static uint32_t GetVertexArrayId() { return m_VertexArray; }

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