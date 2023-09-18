#pragma once
#include "Base.h"
#include "Application/Window/Window.h"
#include "Rendering/Renderer.h"
#include "Application/Scene.h"

class Application
{
public:
	Application();

	void Run();

private:
	Scope<Window> m_Window;
	Scope<Renderer> m_Renderer;

	void handleWindowCommand(WindowCommandEvent command);
	void setupDefaultScene() const;
};