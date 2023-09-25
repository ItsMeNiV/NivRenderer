#pragma once
#include "Base.h"
#include "Application/Scene.h"
#include "Rendering/RenderPass.h"
#include "Rendering/Proxy/SceneObjectProxy.h"
#include "Rendering/Proxy/CameraProxy.h"
#include "Rendering/Proxy/LightProxy.h"
#include "Rendering/Proxy/SkyboxProxy.h"
#include "Rendering/Proxy/ProxyManager.h"
#include "Rendering/OpenGL/Buffer.h"

class ForwardPass : public RenderPass
{
public:
    ForwardPass(Shader* passShader, uint32_t resolutionWidth, uint32_t resolutionHeight, uint32_t sampleCount);

    void Run(Scene* scene, ProxyManager& proxyManager) override;

private:
    Scope<Framebuffer> m_ShadowmapFramebuffer;
    Shader* m_ShadowmapShader;
    Scope<Buffer> m_UniformBufferMatrices;
    Scope<Buffer> m_UniformBufferLight;
    Scope<Buffer> m_UniformBufferSettings;

    void updateShadowmapFramebuffer(Scene* scene);
};
