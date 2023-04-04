#include "NivRenderer.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Entities/SceneObject.h"
#include "Entity/Components/TransformComponent.h"

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
	//Test: Setup Testscene (Remove later)
	Ref<SceneObject> o = CreateRef<SceneObject>();
	m_Scene->AddSceneObject(o);
	o->AddChildEntity(CreateRef<SceneObject>());
	m_Scene->AddSceneObject(CreateRef<SceneObject>());
	m_Scene->AddSceneObject(CreateRef<SceneObject>());
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
