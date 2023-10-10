#pragma once
#include "Base.h"

/*
    RenderCommand:
    ClearColor, ClearColorAndDepth, Draw, DrawIndexed, BlitFramebuffer

    RenderState (size: 3136 bit <-> 392 byte):
    uint32_t BoundVertexArray / In case of BlitFramebuffer BoundReadFramebuffer
    uint32_t BoundShader
    uint32_t BoundWriteFramebuffer (0 means default Framebuffer for viewport we always use the Framebuffer's size) / In case of BlitFramebuffer BoundWriteFramebuffer
    int32_t ReadFramebufferWidth
    int32_t ReadFramebufferHeight
    int32_t WriteFramebufferWidth
    int32_t WriteFramebufferHeight
    TextureUnit BoundTextures[32]
    uint32_t BoundUniformBuffers[5]
    UniformUnit BoundUniforms[5]
    uint32_t Flags

    uint32_t flags: 1 = on, 0 = off
    bit-index-definition
    0 : DEPTH_TEST
    1 : CULL_FACE_FRONT (Cannot be on at the same time as 2)
    2 : CULL_FACE_BACK  (Cannot be on at the same time as 1)
    3 : DEPTH_LESS      (Cannot be on at the same time as 4)
    4 : DEPTH_LEQUAL    (Cannot be on at the same time as 3)
    5 : 
    6 : 
    7 : 
    8 : 
    9 : 
    10 : 
    11 : 
    12 : 
    13 : 
    14 : 
    15 : 
    16 : 
    17 : 
    18 : 
    19 : 
    20 : 
    21 : 
    22 : 
    23 : 
    24 : 
    25 : 
    26 : 
    27 : 
    28 : 
    29 : 
    30 : 
    31 : 
 */

enum class CommandType
{
    CLEAR_COLOR_BUFFER,
    CLEAR_COLOR_DEPTH_BUFFER,
    DRAW, DRAW_INDEXED,
    BLIT_FRAMEBUFFER
};

enum RendererStateFlag
{
    DEPTH_TEST =        SHIFTBITL(0),
    CULL_FACE_FRONT =   SHIFTBITL(1),
    CULL_FACE_BACK =    SHIFTBITL(2),
    DEPTH_LESS =        SHIFTBITL(3),
    DEPTH_LEQUAL =      SHIFTBITL(4)
};

struct TextureUnit
{
    int32_t TextureId = -1;
    int32_t UniformLocation = 0;
};

enum class UniformType
{
    INT, FLOAT, FLOAT2, FLOAT3, FLOAT3X3, FLOAT4X4
};

struct UniformUnit
{
    int32_t Location = -1;
    UniformType Type = UniformType::INT;
    void const* ValuePtr = nullptr;
};

struct RendererState
{
    uint32_t BoundVertexArray = 0;
    uint32_t BoundShader = 0;
    uint32_t BoundWriteFramebuffer = 0;
    int32_t ReadFramebufferWidth = 0;
    int32_t ReadFramebufferHeight = 0;
    int32_t WriteFramebufferWidth = 0;
    int32_t WriteFramebufferHeight = 0;
    TextureUnit BoundTextures[32];
    uint32_t BoundUniformBuffers[5] = {0, 0, 0, 0, 0};
    UniformUnit BoundUniforms[5];
    uint32_t Flags = 0;

    void SetReadFramebuffer(uint32_t framebufferId, int32_t width, int32_t height)
    {
        BoundVertexArray = framebufferId;
        ReadFramebufferWidth = width;
        ReadFramebufferHeight = height;
    }

    void SetWriteFramebuffer(uint32_t framebufferId, int32_t width, int32_t height)
    {
        BoundWriteFramebuffer = framebufferId;
        WriteFramebufferWidth = width;
        WriteFramebufferHeight = height;
    }

    size_t GetHash() const
    {
        return std::hash<uint32_t>{}(BoundShader) | std::hash<uint32_t>{}(BoundVertexArray) |
            std::hash<uint32_t>{}(Flags);
    }
};

struct RenderCommand
{
    CommandType Type;
    RendererState State;
    uint32_t VertexIndexCount;
    size_t StateHash = State.GetHash();
};

constexpr size_t PRE_ALLOC_SIZE = 50;
constexpr size_t RE_ALLOC_SIZE = 50;

struct CommandBuffer
{
    RenderCommand* Buffer;

    CommandBuffer()
    {
        Buffer = new RenderCommand[PRE_ALLOC_SIZE];
        currentItem = Buffer;
    }
    ~CommandBuffer() { delete[] Buffer; }

    size_t Count() const { return (currentItem - Buffer); }
    size_t Size() const { return Count() * sizeof(RenderCommand); }
    void Submit(const RenderCommand& command)
    {
        const size_t currentCount = Count();
        if (currentCount == currentMaxSize)
        {
            currentMaxSize = currentCount + RE_ALLOC_SIZE;
            const auto newBuffer = new RenderCommand[currentMaxSize];
            memcpy(newBuffer, Buffer, (currentCount) * sizeof(RenderCommand));
            delete[] Buffer;
            Buffer = newBuffer;
            currentItem = &newBuffer[currentCount];
        }

        *currentItem = command;
        currentItem++;
    }

private:
    size_t currentMaxSize = PRE_ALLOC_SIZE;
    RenderCommand* currentItem;
};

