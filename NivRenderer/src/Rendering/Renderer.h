#pragma once
#include "Base.h"
#include "Application/Window/Window.h"
#include "Rendering/RenderPipeline.h"
#include "Rendering/Proxy/ProxyManager.h"

class Renderer
{
public:
	Renderer(Window* window);
	~Renderer();

	void PrepareFrame();
	void RenderScene() const;

	Scene* GetScene() const { return m_Scene; }
    void SetScene(Scene* scene) { m_Scene = scene; }
	void SetActivePipeline(RenderPipeline* renderPipeline) { m_ActiveRenderPipeline.reset(renderPipeline); }
    void ResetProxies();

	RenderPipeline* GetActivePipeline() const { return m_ActiveRenderPipeline.get(); }

	void AnimateDirectionalLight() const;
    void UpdatePerformancetestPointLights();

private:
	Window* m_ActiveWindow;
    Scene* m_Scene;
	Scope<RenderPipeline> m_ActiveRenderPipeline;
	Scope<ProxyManager> m_ProxyManager;
    std::unordered_map<uint32_t, float> m_PerformancetestPointLightSpeedMap;
};