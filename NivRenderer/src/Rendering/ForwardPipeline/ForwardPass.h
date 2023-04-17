#pragma once
#include "Base.h"
#include "Rendering/RenderPass.h"

class ForwardPass : public RenderPass
{
public:
    ForwardPass(Ref<Shader> passShader, uint32_t resolutionWidth, uint32_t resolutionHeight)
        : RenderPass(passShader, resolutionWidth, resolutionHeight)
    {
        float vertices[] = {
            -0.5f, -0.5f, 0.0f, // left  
             0.5f, -0.5f, 0.0f, // right 
             0.0f,  0.5f, 0.0f  // top   
        };

        unsigned int vertexBuffer;
        glGenVertexArrays(1, &vertexArray);
        glGenBuffers(1, &vertexBuffer);
        glBindVertexArray(vertexArray);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    virtual void Run()
    {
        m_OutputFramebuffer->Bind();

        //TEST
        glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        m_PassShader->Bind();
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        m_OutputFramebuffer->Unbind();
    }

private:
    unsigned int vertexArray;
    bool firstRun = true;
};