#pragma once
#include "Base.h"
#include "Application/Window/Window.h"
#include "Rendering/RenderPipeline.h"

class Renderer
{
public:
	Renderer(Ref<Window> window, Ref<RenderPipeline> renderPipeline);
	~Renderer();

	void RenderScene();

private:
	Ref<Window> m_ActiveWindow;
	Ref<RenderPipeline> m_ActiveRenderPipeline;

	//TEST
	Ref<Shader> exampleShader;
	unsigned int vertexArray;
};