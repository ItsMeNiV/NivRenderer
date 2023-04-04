#pragma once
#include "Base.h"
#include "Application/Window.h"
#include "Rendering/Renderer.h"
#include "Application/Scene.h"

class Application
{
public:
	Application();

	void Run();

private:
	Ref<Scene> m_Scene;
	Ref<Window> m_Window;
	Ref<Renderer> m_Renderer;
};