#include "OpenGLRenderer3D.h"

void OpenGLRenderer3D::DrawFrame(const CommandBuffer& commandBuffer)
{
    for (uint32_t i = 0; i < commandBuffer.Count(); i++)
    {
        const auto renderCommand = commandBuffer.Buffer[i];
        const auto rendererState = renderCommand.State;

        // Set Framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, rendererState.BoundWriteFramebuffer);
        glViewport(0, 0, rendererState.WriteFramebufferWidth, rendererState.WriteFramebufferHeight);

        // Handle simple command Types first
        GLbitfield clearBits = 0;
        switch (renderCommand.Type)
        {
        case CommandType::CLEAR_COLOR_DEPTH_BUFFER:
            clearBits = GL_DEPTH_BUFFER_BIT;
        case CommandType::CLEAR_COLOR_BUFFER:
            glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
            clearBits |= GL_COLOR_BUFFER_BIT;
            glClear(clearBits);
            break;
        case CommandType::BLIT_FRAMEBUFFER:
            glBlitNamedFramebuffer(rendererState.BoundVertexArray, rendererState.BoundWriteFramebuffer,
                                   0, 0, rendererState.ReadFramebufferWidth, rendererState.ReadFramebufferHeight,
                                   0, 0, rendererState.WriteFramebufferWidth, rendererState.WriteFramebufferHeight,
                                    GL_COLOR_BUFFER_BIT, GL_LINEAR);
            break;
        case CommandType::DRAW:
        case CommandType::DRAW_INDEXED:
        {
                // 1. Setup Render State

                // Set Shader
                glUseProgram(rendererState.BoundShader);

                // Set Uniform Buffers
                for (uint32_t bindingPoint = 0; bindingPoint < 5; bindingPoint++)
                {
                    if (rendererState.BoundUniformBuffers[bindingPoint] != 0)
                        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint,
                                         rendererState.BoundUniformBuffers[bindingPoint]);
                }

                // Set Culling
                if (rendererState.Flags & RendererStateFlag::CULL_FACE_BACK)
                {
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_BACK);
                }
                else if (rendererState.Flags & RendererStateFlag::CULL_FACE_FRONT)
                {
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_FRONT);
                }
                else
                {
                    glDisable(GL_CULL_FACE);
                }

                // Set Depth Test
                if (rendererState.Flags & RendererStateFlag::DEPTH_LESS)
                {
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LESS);
                }
                else if (rendererState.Flags & RendererStateFlag::DEPTH_LEQUAL)
                {
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);
                }
                else
                {
                    glDisable(GL_DEPTH_TEST);
                }

                // Set Textures
                for (uint32_t textureSlot = 0; textureSlot < 32; textureSlot++)
                {
                    if (rendererState.BoundTextures[textureSlot].TextureId != -1)
                    {
                        glBindTextureUnit(textureSlot, rendererState.BoundTextures[textureSlot].TextureId);
                        glProgramUniform1i(rendererState.BoundShader, rendererState.BoundTextures[textureSlot].UniformLocation, textureSlot);
                    }
                }

                // Set Uniforms
                for (uint32_t index = 0; index < 5; index++)
                {
                    const auto uniform = rendererState.BoundUniforms[index];
                    if (uniform.Location == -1)
                        continue;

                    switch (uniform.Type)
                    {
                    case UniformType::INT:
                        glUniform1iv(uniform.Location, 1, static_cast<const GLint*>(uniform.ValuePtr));
                        break;
                    case UniformType::FLOAT:
                        glUniform1fv(uniform.Location, 1, static_cast<const GLfloat*>(uniform.ValuePtr));
                        break;
                    case UniformType::FLOAT2:
                        glUniform2fv(uniform.Location, 1, static_cast<const GLfloat*>(uniform.ValuePtr));
                        break;
                    case UniformType::FLOAT3:
                        glUniform3fv(uniform.Location, 1, static_cast<const GLfloat*>(uniform.ValuePtr));
                        break;
                    case UniformType::FLOAT3X3:
                        glUniformMatrix3fv(uniform.Location, 1, GL_FALSE, static_cast<const GLfloat*>(uniform.ValuePtr));
                        break;
                    case UniformType::FLOAT4X4:
                        glUniformMatrix4fv(uniform.Location, 1, GL_FALSE, static_cast<const GLfloat*>(uniform.ValuePtr));
                        break;
                    }
                }

                // 2. Execute draw
                glBindVertexArray(rendererState.BoundVertexArray);
                if (renderCommand.Type == CommandType::DRAW)
                    glDrawArrays(GL_TRIANGLES, 0, renderCommand.VertexIndexCount);
                else if (renderCommand.Type == CommandType::DRAW_INDEXED)
                    glDrawElements(GL_TRIANGLES, renderCommand.VertexIndexCount, GL_UNSIGNED_INT, nullptr);
            break;   
        }
        }
    }

    // Reset state
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}