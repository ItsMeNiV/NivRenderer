#pragma once
#include "Base.h"
#include <queue>

enum class RenderCommand
{
    SET_CLEAR_COLOR,
    CLEAR_COLOR_BUFFER,
    CLEAR_COLOR_DEPTH_BUFFER,
    SET_VIEWPORT,
    ENABLE_CAPABILITY, SET_CULL_FACE,
    BIND_BUFFER, BIND_FRAMEBUFFER, BIND_SHADERPROGRAM,
    DRAW_BUFFER, DRAW_INDEXED_BUFFER,
};

enum class Capability : GLenum
{
    DEPTHTEST = GL_DEPTH_TEST,
    FACECULLING = GL_CULL_FACE
};

enum class ComparisonFunction : GLenum
{
    NEVER = GL_NEVER, 
    LESS = GL_LESS,
    EQUAL = GL_EQUAL,
    LEQUAL = GL_LEQUAL,
    GREATER = GL_GREATER,
    NOTEQUAL = GL_NOTEQUAL,
    GEQUAL = GL_GEQUAL,
    ALWAYS = GL_ALWAYS
};

constexpr size_t PRE_ALLOC_SIZE = 150;
constexpr size_t RE_ALLOC_SIZE = 50;

struct CommandBuffer
{
    RenderCommand* buffer = new RenderCommand[PRE_ALLOC_SIZE];
    RenderCommand* currentItem = buffer;

    size_t count() const { return (currentItem - buffer); }
    size_t size() const { return count() * sizeof(RenderCommand); }
    void submit(const RenderCommand command)
    {
        const size_t currentCount = count();
        if (currentCount == currentMaxSize)
        {
            currentMaxSize = currentCount + RE_ALLOC_SIZE;
            const auto newBuffer = new RenderCommand[currentMaxSize];
            memcpy(newBuffer, buffer, (currentCount) * sizeof(RenderCommand));
            delete[] buffer;
            buffer = newBuffer;
            currentItem = &newBuffer[currentCount];
        }

        *currentItem = command;
        currentItem++;
    }

private:
    size_t currentMaxSize = PRE_ALLOC_SIZE;
};

