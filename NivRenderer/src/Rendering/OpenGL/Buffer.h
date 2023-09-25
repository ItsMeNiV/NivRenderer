#pragma once
#include "Base.h"

enum class BufferElementType
{
    INT,
    INT2,
    INT3,
    INT4,
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
    FLOAT3X3,
    FLOAT4X4,
    BOOL,
    STRUCT1,
    STRUCT2,
    STRUCT3,
    STRUCT4
};

class BufferLayout
{
public:
    BufferLayout() = default;
    BufferLayout(std::initializer_list<BufferElementType> elements) : m_Elements(elements)
    {
        calculateOffsets();
    }

    const std::vector<BufferElementType>& GetElements() const { return m_Elements; }

    std::vector<BufferElementType>::iterator begin() { return m_Elements.begin(); }
    std::vector<BufferElementType>::iterator end() { return m_Elements.end(); }
    std::vector<BufferElementType>::const_iterator begin() const { return m_Elements.begin(); }
    std::vector<BufferElementType>::const_iterator end() const { return m_Elements.end(); }

private:
    std::vector<BufferElementType> m_Elements;
    std::vector<uint32_t> m_Offsets;

    uint32_t getBaseOffsetByElementType(const BufferElementType& elementType)
    {
        switch (elementType)
        {
        case BufferElementType::INT:
        case BufferElementType::FLOAT:
        case BufferElementType::BOOL:
            return 4;
        case BufferElementType::INT2:
        case BufferElementType::FLOAT2:
            return 8;
        case BufferElementType::INT3:
        case BufferElementType::INT4:
        case BufferElementType::FLOAT3:
        case BufferElementType::FLOAT4:
        case BufferElementType::STRUCT1:
            return 16;
        case BufferElementType::STRUCT2:
            return 32;
        case BufferElementType::STRUCT3:
            return 48;
        case BufferElementType::FLOAT3X3:
        case BufferElementType::FLOAT4X4:
        case BufferElementType::STRUCT4:
            return 64;
        }
    }

    void calculateOffsets()
    {
        uint32_t alignedOffset = 0;
        uint32_t baseOffsetLast = 0;
        for (auto& element : m_Elements)
        {
            const uint32_t baseOffset = getBaseOffsetByElementType(element);
            alignedOffset += baseOffsetLast;

            if (alignedOffset != 0 || alignedOffset % baseOffset != 0)
            {
                while (alignedOffset % baseOffset != 0)
                    alignedOffset++;
            }

            m_Offsets.push_back(alignedOffset);

            baseOffsetLast = baseOffset;
        }
    }
};

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
    Buffer(const BufferType bufferType, const BufferLayout& bufferLayout);
    ~Buffer();

    void BufferData(const void* data, BufferElementType dataType, uint32_t elementOffset) const;
    void BufferData(const void* data, size_t dataSize, size_t bufferOffset = -1) const;
    void BindUniformBufferToBindingPoint(uint32_t bindingPoint, int32_t rangeFrom = -1, int32_t rangeTo = -1) const;

private:
    uint32_t m_Id;
    BufferType m_BufferType;
    BufferLayout m_BufferLayout;

    static uint32_t GetUniformBufferElementSize(const BufferElementType elementType)
    {
        switch (elementType)
        {
        case BufferElementType::INT:
        case BufferElementType::BOOL:
        case BufferElementType::FLOAT:
            return 4;
        case BufferElementType::FLOAT2:
        case BufferElementType::FLOAT3:
        case BufferElementType::FLOAT4:
            return 16;
        default:
            return 0;
        }
    }
};