#pragma once
#include "Base.h"

enum BufferType : GLenum
{
    VertexBuffer = GL_ARRAY_BUFFER,
    IndexBuffer = GL_ELEMENT_ARRAY_BUFFER,
    UniformBuffer = GL_UNIFORM_BUFFER
};

class Buffer
{
public:
    Buffer(const BufferType bufferType);
    ~Buffer();

    void BufferData(const void* data, size_t dataSize, size_t bufferOffset = -1) const;
    void BindUniformBufferToBindingPoint(uint32_t bindingPoint, int32_t rangeFrom = -1, int32_t rangeTo = -1) const;

private:
    uint32_t m_Id;
    BufferType m_BufferType;

};