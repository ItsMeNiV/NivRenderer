#pragma once
#include "Base.h"
#include "Application/Window/Window.h"
#include "Rendering/RenderPipeline.h"
#include "Rendering/Proxy/ProxyManager.h"

class Renderer
{
public:
	Renderer(Ref<Window> window);
	~Renderer();

	void PrepareFrame();
	void RenderScene();

	void SetActiveScene(const Ref<Scene> scene) { m_ActiveScene = scene; }
	void SetActivePipeline(const Ref<RenderPipeline> renderPipeline) { m_ActiveRenderPipeline = renderPipeline; }

private:
	Ref<Window> m_ActiveWindow;
	Ref<Scene> m_ActiveScene;
	Ref<RenderPipeline> m_ActiveRenderPipeline;
	Scope<ProxyManager> m_ProxyManager;
};