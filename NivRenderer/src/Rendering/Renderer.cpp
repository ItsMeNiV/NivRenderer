#include "Renderer.h"
#include "Entity/ECSRegistry.h"

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
    Ref<Camera> cam = ECSRegistry::GetInstance().GetEntity<CameraObject>(m_ActiveScene->GetCameraId())->GetCameraPtr();
    if(m_ActiveScene->GetSceneSettings().renderResolution.x != cam->GetCameraWidth() ||
        m_ActiveScene->GetSceneSettings().renderResolution.y != cam->GetCameraHeight())
    {
        cam->UpdateWindowSize(m_ActiveScene->GetSceneSettings().renderResolution.x,
                              m_ActiveScene->GetSceneSettings().renderResolution.y);
        m_ActiveRenderPipeline->UpdateResolution(m_ActiveScene->GetSceneSettings().renderResolution.x,
                                                 m_ActiveScene->GetSceneSettings().renderResolution.y);
    }

    if (m_ActiveScene->GetSceneSettings().sampleCount != m_ActiveRenderPipeline->GetSampleCount())
        m_ActiveRenderPipeline->UpdateSampleCount(m_ActiveScene->GetSceneSettings().sampleCount);

    m_ProxyManager->UpdateProxies(m_ActiveScene);
}

void Renderer::RenderScene()
{
    if (m_ActiveScene)
    {
        auto& outputFramebuffer = m_ActiveRenderPipeline->Run(m_ActiveScene, *m_ProxyManager);
        outputFramebuffer.BlitFramebuffer(m_ActiveWindow->GetFramebuffer()->GetId(),
                                          m_ActiveWindow->GetFramebuffer()->GetWidth(),
                                          m_ActiveWindow->GetFramebuffer()->GetHeight());
    }

}
