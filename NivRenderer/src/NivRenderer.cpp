#include "NivRenderer.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Entities/GameObject.h"
#include "Entity/Components/TransformComponent.h"

int main()
{
	Application* app = new Application();

	app->Run();

	delete app;
}

Application::Application()
	: m_Window(CreateRef<Window>(1600, 900, "NivRenderer")), m_Renderer(CreateRef<Renderer>(m_Window))
{}

void Application::Run()
{
	Ref<GameObject> e = CreateRef<GameObject>();
	Ref<TransformComponent> c1 = CreateRef<TransformComponent>();
	ECSRegistry::GetInstance().AddEntity(e);
	ECSRegistry::GetInstance().AddComponent(e->GetId(), c1);
	Ref<TransformComponent> c2 = ECSRegistry::GetInstance().GetComponent<TransformComponent>(e->GetId());
	ECSRegistry::GetInstance().RemoveEntity(e->GetId());

	while (!m_Window->ShouldClose())
	{
		//Handle inputs
		m_Renderer->RenderScene();

		m_Window->PollEvents();
		m_Window->SwapBuffers();
	}
}
