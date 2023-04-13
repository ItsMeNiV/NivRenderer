#include "Renderer.h"

Renderer::Renderer(Ref<Window> window, Ref<RenderPipeline> renderPipeline)
	: m_ActiveWindow(window), m_ActiveRenderPipeline(renderPipeline), m_ProxyManager(CreateScope<ProxyManager>())
{
	window->CreateRenderContext();

    //TEST
    exampleShader = CreateRef<Shader>("assets/exampleshader.glsl", ShaderType::VERTEX_AND_FRAGMENT);

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
    //TEST
}

Renderer::~Renderer()
{
}

void Renderer::PrepareFrame()
{
    m_ProxyManager->UpdateProxies(m_ActiveScene);
}

void Renderer::RenderScene()
{
	m_ActiveWindow->GetFramebuffer()->Bind();
	glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    if (m_ActiveScene)
    {
        //TEST
        exampleShader->Bind();
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //TEST
    }

	m_ActiveWindow->GetFramebuffer()->Unbind();
}
