#include "Buffer.h"

Buffer::Buffer(const BufferType bufferType)
    : m_BufferType(bufferType)
{
    glCreateBuffers(1, &m_Id);
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &m_Id);
}

void Buffer::BufferData(const void* data, size_t dataSize, size_t bufferOffset) const
{
    if (bufferOffset == -1)
        glNamedBufferData(m_Id, dataSize, data, GL_DYNAMIC_DRAW);
    else
        glNamedBufferSubData(m_Id, bufferOffset, dataSize, data);
}

void Buffer::BindUniformBufferToBindingPoint(uint32_t bindingPoint, int32_t rangeFrom, int32_t rangeTo) const
{
    if (m_BufferType != BufferType::UniformBuffer)
        return;

    if (rangeFrom == -1 || rangeTo == -1)
        glBindBufferBase(m_BufferType, bindingPoint, m_Id);
    else
        glBindBufferRange(m_BufferType, bindingPoint, m_Id, rangeFrom, rangeTo);
}
