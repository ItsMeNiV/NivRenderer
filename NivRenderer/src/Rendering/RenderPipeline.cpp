#include "RenderPipeline.h"

RenderPipeline::RenderPipeline(std::vector<Ref<RenderPass>> renderPasses, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount)
    : m_RenderPasses(renderPasses), m_Framebuffer(nullptr), m_ResolutionWidth(resolutionWidth), m_ResolutionHeight(resolutionHeight), m_SampleCount(sampleCount)
{
}

Framebuffer& RenderPipeline::Run(Ref<Scene> scene, const ProxyManager& proxyManager)
{
    //TEST
    if (firstRun)
    {
        m_Framebuffer = CreateScope<Framebuffer>(m_ResolutionWidth, m_ResolutionHeight, m_SampleCount);
        exampleShader = CreateRef<Shader>("assets/shaders/exampleshader.glsl", ShaderType::VERTEX_AND_FRAGMENT);

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

        firstRun = false;
        //TEST
    }
    m_Framebuffer->Bind();
    for (auto currentPass : m_RenderPasses)
    {
        //Map input
        //Run pass
        //Get output
    }

    //TEST
    glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    exampleShader->Bind();
    glBindVertexArray(vertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    m_Framebuffer->Unbind();

    return *m_Framebuffer;
}
