#include "Renderer.h"
#include "Application/Util/Instrumentor.h"
#include "Entity/NewECSRegistry.h"
#include "Rendering/OpenGL/OpenGLRenderer3D.h"

Renderer::Renderer(Window* window)
	: m_ActiveWindow(window), m_Scene(nullptr), m_ActiveRenderPipeline(nullptr), m_ProxyManager(CreateScope<ProxyManager>())
{
	window->CreateRenderContext();
}

Renderer::~Renderer()
{
}

void Renderer::PrepareFrame() const
{
    PROFILE_FUNCTION()
    const auto cam = NewECSRegistry::GetInstance().GetComponent<NewComponents::CameraComponent>(m_Scene->GetActiveCameraId())->cameraPtr;
    if(m_Scene->GetSceneSettings().renderResolution.x != cam->GetCameraWidth() ||
        m_Scene->GetSceneSettings().renderResolution.y != cam->GetCameraHeight())
    {
        cam->UpdateWindowSize(m_Scene->GetSceneSettings().renderResolution.x,
                              m_Scene->GetSceneSettings().renderResolution.y);
        m_ActiveRenderPipeline->UpdateResolution(m_Scene->GetSceneSettings().renderResolution.x,
                                                 m_Scene->GetSceneSettings().renderResolution.y);
    }

    if (m_Scene->GetSceneSettings().sampleCount != m_ActiveRenderPipeline->GetSampleCount())
        m_ActiveRenderPipeline->UpdateSampleCount(m_Scene->GetSceneSettings().sampleCount);

    if (m_Scene->GetSceneSettings().animateDirectionalLight)
        AnimateDirectionalLight();

    m_ProxyManager->UpdateProxies(m_Scene);
}

void Renderer::RenderScene() const
{
    PROFILE_FUNCTION()
    if (m_Scene)
    {
        CommandBuffer commandBuffer;
        const auto& outputFramebuffer = m_ActiveRenderPipeline->Run(m_Scene, *m_ProxyManager, commandBuffer);
        RendererState rendererState;
        rendererState.SetReadFramebuffer(outputFramebuffer.GetId(), outputFramebuffer.GetWidth(),
                                         outputFramebuffer.GetHeight());
        rendererState.SetWriteFramebuffer(m_ActiveWindow->GetFramebuffer()->GetId(),
                                          m_ActiveWindow->GetFramebuffer()->GetWidth(),
                                          m_ActiveWindow->GetFramebuffer()->GetHeight());
        commandBuffer.Submit({CommandType::BLIT_FRAMEBUFFER, rendererState, 0});
        OpenGLRenderer3D::DrawFrame(commandBuffer);
    }

}

void Renderer::AnimateDirectionalLight() const
{
    if (const auto directionalLightObject = NewECSRegistry::GetInstance().GetEntity(m_Scene->GetDirectionalLightId()))
    {
        const auto dLightComponent = NewECSRegistry::GetInstance().GetComponent<NewComponents::DirectionalLightComponent>(m_Scene->GetDirectionalLightId());
        const double time = m_ActiveWindow->GetWindowRuntime();
        dLightComponent->direction = glm::normalize(glm::vec3(-(1 + sin(time) * 3), -3, -(1 + cos(time) * 3)));
        dLightComponent->dirtyFlag = true;
    }
}
