#include "Renderer.h"

Renderer::Renderer(Ref<Window> window, Ref<RenderPipeline> renderPipeline)
	: m_ActiveWindow(window), m_ActiveRenderPipeline(renderPipeline), m_ProxyManager(CreateScope<ProxyManager>())
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
