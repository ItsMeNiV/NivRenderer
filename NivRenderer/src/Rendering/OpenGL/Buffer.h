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
    STRUCT_START,
    STRUCT_END
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
    const std::vector<uint32_t>& GetOffsets() const { return m_Offsets; }

    std::vector<BufferElementType>::iterator begin() { return m_Elements.begin(); }
    std::vector<BufferElementType>::iterator end() { return m_Elements.end(); }
    std::vector<BufferElementType>::const_iterator begin() const { return m_Elements.begin(); }
    std::vector<BufferElementType>::const_iterator end() const { return m_Elements.end(); }

    size_t GetLayoutSize() const
    {
        size_t size = 0;
        for (auto& element : m_Elements)
        {
            size += getSizeOfElementType(element);
        }
        return size;
    }

private:
    std::vector<BufferElementType> m_Elements;
    std::vector<uint32_t> m_Offsets;

    static uint32_t getAlignmentOfElementType(const BufferElementType& elementType)
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
        case BufferElementType::FLOAT3X3:
        case BufferElementType::FLOAT4X4:
            return 16;
        default:
            return 0;
        }
    }
    static uint32_t getSizeOfElementType(const BufferElementType& elementType)
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
        case BufferElementType::FLOAT3:
            return 12;
        case BufferElementType::INT4:
        case BufferElementType::FLOAT4:
            return 16;
        case BufferElementType::FLOAT3X3:
            return 36;
        case BufferElementType::FLOAT4X4:
            return 64;
        default:
            return 0;
        }
    }

    void calculateOffsets()
    {
        uint32_t alignedOffset = 0;
        for (auto& element : m_Elements)
        {
            if (element == BufferElementType::STRUCT_START)
                continue;

            if (element == BufferElementType::STRUCT_END)
            {
                //Add Padding
                while (alignedOffset % getAlignmentOfElementType(BufferElementType::FLOAT4) != 0)
                    alignedOffset++;
                continue;
            }

            const uint32_t elementAlignment = getAlignmentOfElementType(element);

            if (alignedOffset != 0 && alignedOffset % elementAlignment != 0)
            {
                while (alignedOffset % elementAlignment != 0)
                    alignedOffset++;
            }

            m_Offsets.push_back(alignedOffset);
            alignedOffset += getSizeOfElementType(element);
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
    ~Buffer();

    void SetBufferLayout(const std::initializer_list<BufferElementType> elements);
    void BufferData(const void* data, size_t dataSize, uint32_t bufferElementOffset = -1) const;
    void BindUniformBufferToBindingPoint(uint32_t bindingPoint, int32_t rangeFrom = -1, int32_t rangeTo = -1) const;
    uint32_t GetId() const { return m_Id; }

private:
    uint32_t m_Id;
    BufferType m_BufferType;
    BufferLayout m_BufferLayout;
};