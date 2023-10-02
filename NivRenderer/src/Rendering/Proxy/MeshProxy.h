#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"
#include "Entity/Components/MeshComponent.h"

class MeshProxy : public Proxy
{
public:
    MeshProxy(uint32_t id);
    ~MeshProxy() override;

    void CreateBuffers(const MeshComponent* const mesh);

    void Bind() const;
    static void Unbind();

    uint32_t GetIndexCount() const { return m_IndexCount; }
    uint32_t GetVerticesCount() const { return m_VerticesCount; }
    uint32_t GetVertexArrayId() const { return m_VertexArray; }

private:
    // TODO: Abstract
    uint32_t m_VertexArray, m_VertexBuffer, m_IndexBuffer;
    uint32_t m_IndexCount, m_VerticesCount;
};