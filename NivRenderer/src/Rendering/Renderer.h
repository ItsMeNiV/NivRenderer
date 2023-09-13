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

	void PrepareFrame() const;
	void RenderScene() const;

	void SetActiveScene(Ref<Scene> scene) { m_ActiveScene = scene; }
	void SetActivePipeline(const Ref<RenderPipeline>& renderPipeline) { m_ActiveRenderPipeline = renderPipeline; }

	const Ref<RenderPipeline>& GetActivePipeline() const { return m_ActiveRenderPipeline; }

	void AnimateDirectionalLight() const;

private:
	Ref<Window> m_ActiveWindow;
	Ref<Scene> m_ActiveScene;
	Ref<RenderPipeline> m_ActiveRenderPipeline;
	Scope<ProxyManager> m_ProxyManager;
};