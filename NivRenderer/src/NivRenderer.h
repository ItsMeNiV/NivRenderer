#pragma once
#include "Base.h"
#include "Application/Window.h"
#include "Rendering/Renderer.h"

class Application
{
public:
	Application();

	void Run();

private:
	Ref<Window> m_Window;
	Ref<Renderer> m_Renderer;
};