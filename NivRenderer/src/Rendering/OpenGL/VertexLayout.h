#pragma once
#include "Base.h"

enum VertexType
{
    INT, INT2, INT3, INT4, FLOAT, FLOAT2, FLOAT3, FLOAT4, FLOAT3X3, FLOAT4X4, BOOL
};

static uint32_t OpenGLVertexType(VertexType type) const
{
    switch (type)
    {
    case VertexType::INT: return GL_INT;
    case VertexType::INT2: return GL_INT;
    case VertexType::INT3: return GL_INT;
    case VertexType::INT4: return GL_INT;
    case VertexType::FLOAT: return GL_FLOAT;
    case VertexType::FLOAT2: return GL_FLOAT;
    case VertexType::FLOAT3: return GL_FLOAT;
    case VertexType::FLOAT4: return GL_FLOAT;
    case VertexType::FLOAT3X3: return GL_FLOAT;
    case VertexType::FLOAT4X4: return GL_FLOAT;
    case VertexType::BOOL: return GL_BOOL;
    }

    return 0;
}

static uint32_t VertexTypeSize(VertexType type) const
{
    switch (type)
    {
    case VertexType::INT: return 4;
    case VertexType::INT2: return 4 * 2;
    case VertexType::INT3: return 4 * 3;
    case VertexType::INT4: return 4 * 4;
    case VertexType::FLOAT: return 4;
    case VertexType::FLOAT2: return 4 * 2;
    case VertexType::FLOAT3: return 4 * 3;
    case VertexType::FLOAT4: return 4 * 4;
    case VertexType::FLOAT3X3: return 4 * 3 * 3;
    case VertexType::FLOAT4X4: return 4 * 4 * 4;
    case VertexType::BOOL: return 1;
    }

    return 0;
}

struct VertexElement
{
    std::string Name;
    VertexType Type;
    uint32_t Size;
    size_t Offset;
    bool Normalized;

    VertexElement() = default;

    VertexElement(VertexType type, const std::string& name, bool normalized = false)
        : Name(name), Type(type), Size(VertexTypeSize(type)), Offset(0), Normalized(normalized)
    {}

    uint32_t GetComponentCount() const
    {
        switch (type)
        {
        case VertexType::INT: return 1;
        case VertexType::INT2: return 2;
        case VertexType::INT3: return 3;
        case VertexType::INT4: return 4;
        case VertexType::FLOAT: return 1;
        case VertexType::FLOAT2: return 2;
        case VertexType::FLOAT3: return 3;
        case VertexType::FLOAT4: return 4;
        case VertexType::FLOAT3X3: return 3;
        case VertexType::FLOAT4X4: return 4;
        case VertexType::BOOL: return 1;
        }

        return 0;
    }
};

class VertexLayout
{
public:
    VertexLayout() = default;
    VertexLayout(std::initializer_list<VertexElement> elements)
        : elements(elements)
    {
        calculateOffsetsAndStride();
    }

    const uint32_t GetStride() const { return m_Stride; }
    const std::vector<VertexElement>& GetElements() const { return m_Elements; }

    std::vector<VertexElement>::iterator begin() { return m_Elements.begin(); }
    std::vector<VertexElement>::iterator end() { return m_Elements.end(); }
    std::vector<VertexElement>::const_iterator begin() const { return m_Elements.begin(); }
    std::vector<VertexElement>::const_iterator end() const { return m_Elements.end(); }

private:
    std::vector<VertexElement> m_Elements;
    uint32_t m_Stride = 0;

    void calculateOffsetsAndStride()
    {
        size_t offset = 0;
        m_Stride = 0;
        for (auto& element : elements)
        {
            element.Offset = offset;
            offset += element.Size;
            m_Stride += element.Size;
        }
    }

};