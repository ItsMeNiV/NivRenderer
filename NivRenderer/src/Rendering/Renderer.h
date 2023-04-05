#pragma once
#include "Base.h"
#include "Application/Window/Window.h"

class Renderer
{
public:
	Renderer(Ref<Window> window);
	~Renderer();

	void RenderScene();

private:
	Ref<Window> m_ActiveWindow;

	float rColor = 0.0f;
};