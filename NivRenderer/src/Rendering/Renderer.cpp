#include "Renderer.h"
#include "Entity/ECSRegistry.h"
#include "Application/Util/Instrumentor.h"

Renderer::Renderer(Ref<Window> window)
	: m_ActiveWindow(window), m_ActiveScene(nullptr), m_ActiveRenderPipeline(nullptr), m_ProxyManager(CreateScope<ProxyManager>())
{
	window->CreateRenderContext();
}

Renderer::~Renderer()
{
}

void Renderer::PrepareFrame() const
{
    PROFILE_FUNCTION()
    const Ref<Camera> cam = ECSRegistry::GetInstance().GetEntity<CameraObject>(m_ActiveScene->GetCameraId())->GetCameraPtr();
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

    if (m_ActiveScene->GetSceneSettings().animateDirectionalLight)
        AnimateDirectionalLight();

    m_ProxyManager->UpdateProxies(m_ActiveScene);
}

void Renderer::RenderScene() const
{
    PROFILE_FUNCTION()
    if (m_ActiveScene)
    {
        auto& outputFramebuffer = m_ActiveRenderPipeline->Run(m_ActiveScene, *m_ProxyManager);
        outputFramebuffer.BlitFramebuffer(m_ActiveWindow->GetFramebuffer()->GetId(),
                                          m_ActiveWindow->GetFramebuffer()->GetWidth(),
                                          m_ActiveWindow->GetFramebuffer()->GetHeight());
    }

}

void Renderer::AnimateDirectionalLight() const
{
    for(uint32_t lightId : m_ActiveScene->GetSceneLightIds())
    {
        const Ref<DirectionalLightObject> directionalLightObject =
            ECSRegistry::GetInstance().GetEntity<DirectionalLightObject>(lightId);
        if (directionalLightObject)
        {
            auto& direction = directionalLightObject->GetDirection();
            const double time = m_ActiveWindow->GetWindowRuntime();
            direction = glm::normalize(glm::vec3(-(1+sin(time) * 3), -3, -(1+cos(time) * 3)));
            directionalLightObject->SetDirtyFlag(true);
        }
    }
}
