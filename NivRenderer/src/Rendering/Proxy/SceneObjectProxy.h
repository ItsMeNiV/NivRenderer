#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"
#include "Entity/Components/MeshComponent.h"
#include "Entity/Components/MaterialComponent.h"

class SceneObjectProxy : public Proxy
{
public:
    SceneObjectProxy(uint32_t id);

    ~SceneObjectProxy() override;

    void SetTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);
    void SetMesh(const Ref<MeshComponent>& mesh);
    void SetMaterial(const Ref<MaterialComponent>& material);
    void Bind() const;
    void Unbind() const;
    bool HasDiffuseTexture() const;
    bool HasSpecularTexture() const;
    void BindDiffuseTexture(const int32_t slot) const;
    void BindSpecularTexture(const int32_t slot) const;

    const uint32_t GetIndexCount() const { return m_IndexCount; }
    const uint32_t GetVerticesCount() const { return m_VerticesCount; }
    glm::mat4& GetModelMatrix() { return m_ModelMatrix; }

private:
    glm::mat4 m_ModelMatrix;
    uint32_t m_DiffuseTexture, m_SpecularTexture;

    //TODO: Abstract
    uint32_t m_VertexArray, m_VertexBuffer, m_IndexBuffer;
    uint32_t m_IndexCount, m_VerticesCount;

    void createTextureFromAsset(const Ref<TextureAsset>& textureAsset, uint32_t* textureId);
};