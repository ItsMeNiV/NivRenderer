#include "NivRenderer.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Entities/SceneObject.h"
#include "Entity/Components/TransformComponent.h"
#include "Rendering/ForwardPipeline/ForwardPass.h"
#include "Application/Util/Instrumentor.h"
#include "Application/Serialization/SerializationManager.h"

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
	: m_Window(CreateScope<Window>(1600, 900, "NivRenderer"))
{
	m_Window->SetCommandHandler([&](WindowCommandEvent command) { handleWindowCommand(command); });

	m_Renderer = CreateScope<Renderer>(m_Window.get());
    Scene* scene = m_Renderer->GetScene();

	setupDefaultScene();

	std::vector<Scope<RenderPass>> renderPasses;
    renderPasses.push_back(
        CreateScope<ForwardPass>(AssetManager::GetInstance().LoadShader(
                                     std::string("assets/shaders/forwardrender.glsl"), ShaderType::VERTEX_AND_FRAGMENT),
                                 scene->GetSceneSettings().renderResolution.x,
                                 scene->GetSceneSettings().renderResolution.y, scene->GetSceneSettings().sampleCount));
    std::vector<Scope<RenderPass>> postProcessingPasses;

	m_Renderer->SetActivePipeline(new RenderPipeline(renderPasses, postProcessingPasses, scene->GetSceneSettings().renderResolution.x, scene->GetSceneSettings().renderResolution.y));
}

void Application::Run()
{
	while (!m_Window->ShouldClose())
	{
        PROFILE_FUNCTION()
        m_Window->HandleWindowCommands();
		m_Window->PollEvents();
		m_Window->PrepareFrame();

		m_Window->RenderImGui(m_Renderer->GetScene());

		m_Window->ProcessInput();
		m_Renderer->PrepareFrame();
		m_Renderer->RenderScene();

		m_Window->SwapBuffers();
	}
}

void Application::handleWindowCommand(WindowCommandEvent command)
{
	if (command.GetCommand() == WindowCommand::RecompileShaders)
		m_Renderer->GetActivePipeline()->RecompileShaders();
    if (command.GetCommand() == WindowCommand::SaveScene)
        SerializationManager::SaveSceneToFile("default.json", m_Renderer->GetScene());
    if (command.GetCommand() == WindowCommand::LoadScene)
    {
        Scene* newScene = SerializationManager::LoadSceneFromFile("default.json");
        newScene->AddCamera(m_Window->GetCamera());
        m_Renderer->SetScene(newScene);
    }
}

void Application::setupDefaultScene() const
{
    Scene* scene = m_Renderer->GetScene();
    scene->AddSceneObject();
    scene->AddDirectionalLight();
    m_Window->CreateCameraAndController(scene->GetSceneSettings().renderResolution);
    scene->AddCamera(m_Window->GetCamera());
}
