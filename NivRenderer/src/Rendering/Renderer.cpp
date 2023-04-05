#include "Renderer.h"

Renderer::Renderer(Ref<Window> window)
	: m_ActiveWindow(window)
{
	window->CreateRenderContext();
}

Renderer::~Renderer()
{
}

void Renderer::RenderScene()
{
	m_ActiveWindow->GetFramebuffer()->Bind();
	glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_ActiveWindow->GetFramebuffer()->Unbind();
}
