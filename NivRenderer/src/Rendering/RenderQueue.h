#pragma once
#include "Base.h"
#include <queue>

/*
    RenderCommand:
    ClearColor, ClearColorAndDepth, Draw, DrawIndexed

    RenderState (size: 160 bit <-> 20 byte):
    uint32_t BoundVertexArray
    uint32_t BoundUniformBuffer
    uint32_t BoundShader
    uint32_t  BoundFramebuffer; (0 means default framebuffer for viewport we always use the framebuffer's size)

    uint32_t flags: 1 = on, 0 = off
    bit-index-definition
    0 : DEPTH_TEST
    1 : CULL_FACE_FRONT (Cannot be on at the same time as 2)
    2 : CULL_FACE_BACK  (Cannot be on at the same time as 1)
    3 : 
    4 : 
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
};

enum RendererStateFlag
{
    DEPTH_TEST =        SHIFTBITL(0),
    CULL_FACE_FRONT =   SHIFTBITL(1),
    CULL_FACE_BACK =    SHIFTBITL(2)
};

struct RendererState
{
    uint32_t BoundVertexArray = 0;
    uint32_t BoundUniformBuffer = 0;
    uint32_t BoundShader = 0;
    uint32_t BoundFramebuffer = 0;
    uint32_t Flags = 0;
};

struct RenderCommand
{
    CommandType Type;
    RendererState State;
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

