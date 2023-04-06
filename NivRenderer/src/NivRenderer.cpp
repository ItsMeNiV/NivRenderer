#include "NivRenderer.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Entities/SceneObject.h"
#include "Entity/Components/TransformComponent.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

int main()
{
	Application* app = new Application();

	app->Run();

	delete app;
}

Application::Application()
	: m_Window(CreateRef<Window>(1600, 900, "NivRenderer")), m_Renderer(CreateRef<Renderer>(m_Window)), m_Scene(CreateRef<Scene>())
{
	//Setup Testscene (TODO: Remove later)
	uint32_t oId = m_Scene->AddSceneObject();
	m_Scene->AddSceneObject(oId);
	m_Scene->AddSceneObject();
	m_Scene->AddSceneObject();
}

void Application::Run()
{
	while (!m_Window->ShouldClose())
	{
		m_Window->PollEvents();
		m_Window->PrepareFrame();

		m_Window->RenderImGui(m_Scene);

		//Handle inputs
		m_Renderer->RenderScene();
		

		m_Window->SwapBuffers();
	}
}
