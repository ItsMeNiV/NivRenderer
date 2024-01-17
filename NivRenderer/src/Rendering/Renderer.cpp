#include "Renderer.h"
#include "Application/Util/Instrumentor.h"
#include "Entity/ECSRegistry.h"
#include "Rendering/OpenGL/OpenGLRenderer3D.h"

Renderer::Renderer(Window* window)
	: m_ActiveWindow(window), m_Scene(nullptr), m_ActiveRenderPipeline(nullptr), m_ProxyManager(CreateScope<ProxyManager>())
{
	window->CreateRenderContext();
}

Renderer::~Renderer()
{
}

void Renderer::PrepareFrame()
{
    PROFILE_FUNCTION()
    const auto cam = ECSRegistry::GetInstance().GetComponent<CameraComponent>(m_Scene->GetActiveCameraId())->cameraPtr;
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
    if (m_Scene->GetSceneSettings().performanceTest)
        UpdatePerformancetestPointLights();

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

void Renderer::ResetProxies() { m_ProxyManager->Reset(); }

void Renderer::AnimateDirectionalLight() const
{
    if (const auto directionalLightObject = ECSRegistry::GetInstance().GetEntity(m_Scene->GetDirectionalLightId()))
    {
        const auto dLightComponent = ECSRegistry::GetInstance().GetComponent<DirectionalLightComponent>(m_Scene->GetDirectionalLightId());
        const double time = m_ActiveWindow->GetWindowRuntime();
        dLightComponent->direction = glm::normalize(glm::vec3(-(1 + sin(time) * 3), -3, -(1 + cos(time) * 3)));
        dLightComponent->dirtyFlag = true;
    }
}

void Renderer::UpdatePerformancetestPointLights()
{
    if (m_PerformancetestPointLightSpeedMap.empty())
    {
        for (uint16_t i = 0; i < 10; i++)
        {
            uint32_t lightId = m_Scene->AddPointLight();
            const float speed = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            m_PerformancetestPointLightSpeedMap[lightId] = (speed / 10.0f) + 0.025;
            const auto lightComponent = ECSRegistry::GetInstance().GetComponent<PointLightComponent>(lightId);
            lightComponent->position = {(rand() % 20) - 10.0f, -5.0f, (rand() % 20) - 10.0f};
        }
    }

    for (const auto& light : m_PerformancetestPointLightSpeedMap)
    {
        const auto lightComponent = ECSRegistry::GetInstance().GetComponent<PointLightComponent>(light.first);
        lightComponent->position.y += light.second;
        lightComponent->dirtyFlag = true;
        if (lightComponent->position.y >= 10.0f)
            lightComponent->position = {(rand() % 20) - 10.0f, -5.0f, (rand() % 20) - 10.0f};
    }
}
