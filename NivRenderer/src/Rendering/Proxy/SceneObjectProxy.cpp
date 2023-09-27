#include "Rendering/Proxy/SceneObjectProxy.h"

SceneObjectProxy::SceneObjectProxy(uint32_t id) :
    Proxy(id), m_MeshProxy(nullptr), m_MaterialProxy(nullptr), m_ModelMatrix(1.0f)
{}

SceneObjectProxy::~SceneObjectProxy() = default;

void SceneObjectProxy::SetTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    m_ModelMatrix = glm::mat4(1.0f);
    const glm::mat4 translationMatrix = glm::translate(m_ModelMatrix, position);
    const glm::mat4 scalingMatrix = glm::scale(m_ModelMatrix, scale);
    const glm::mat4 rotationX = glm::rotate(m_ModelMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 rotationY = glm::rotate(m_ModelMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 rotationZ = glm::rotate(m_ModelMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::mat4 rotationMatrix = rotationY * rotationX * rotationZ;

    m_ModelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
}

void SceneObjectProxy::SetMesh(MeshProxy* const meshProxy)
{
    m_MeshProxy = meshProxy;
}

void SceneObjectProxy::SetMaterial(MaterialProxy* const materialProxy)
{
    m_MaterialProxy = materialProxy;
}

void SceneObjectProxy::Bind() const
{
    m_MeshProxy->Bind();
}

void SceneObjectProxy::Unbind() const
{
    m_MeshProxy->Unbind();
}
