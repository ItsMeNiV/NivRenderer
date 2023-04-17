#include "Renderer.h"

Renderer::Renderer(Ref<Window> window)
	: m_ActiveWindow(window), m_ActiveScene(nullptr), m_ActiveRenderPipeline(nullptr), m_ProxyManager(CreateScope<ProxyManager>())
{
	window->CreateRenderContext();
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
    if (m_ActiveScene)
    {
        auto& outputFramebuffer = m_ActiveRenderPipeline->Run(m_ActiveScene, *m_ProxyManager);
        outputFramebuffer.BlitFramebuffer(m_ActiveWindow->GetFramebuffer()->GetId());
    }

}
