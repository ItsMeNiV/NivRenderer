#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"
#include "MaterialProxy.h"
#include "MeshProxy.h"

class SceneObjectProxy : public Proxy
{
public:
    SceneObjectProxy(uint32_t id);

    ~SceneObjectProxy() override;

    void SetTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);
    void SetMesh(const Ref<MeshProxy>& meshProxy);
    void SetMaterial(const Ref<MaterialProxy>& materialProxy);
    void Bind() const;
    void Unbind() const;

    glm::mat4& GetModelMatrix() { return m_ModelMatrix; }
    const Ref<MeshProxy>& GetMeshProxy() const { return m_MeshProxy; }
    const Ref<MaterialProxy>& GetMaterialProxy() const { return m_MaterialProxy; }

private:
    glm::mat4 m_ModelMatrix;
    Ref<MaterialProxy> m_MaterialProxy;
    Ref<MeshProxy> m_MeshProxy;
};