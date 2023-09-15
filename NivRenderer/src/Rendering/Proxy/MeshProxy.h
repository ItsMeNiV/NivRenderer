#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"
#include "Entity/Components/MeshComponent.h"

class MeshProxy : public Proxy
{
public:
    MeshProxy(uint32_t id);
    ~MeshProxy();

    void CreateBuffers(MeshComponent* const mesh);

    void Bind() const;
    void Unbind() const;

    const uint32_t GetIndexCount() const { return m_IndexCount; }
    const uint32_t GetVerticesCount() const { return m_VerticesCount; }

private:
    // TODO: Abstract
    uint32_t m_VertexArray, m_VertexBuffer, m_IndexBuffer;
    uint32_t m_IndexCount, m_VerticesCount;
};