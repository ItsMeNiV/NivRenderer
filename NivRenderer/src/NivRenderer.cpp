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

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_ERROR

int main()
{
    spdlog::set_level(spdlog::level::err);

    const auto buffer = new CommandBuffer();

    for (int i = 0; i <= 300; i++)
    {
        buffer->Submit({CommandType::DRAW, {}});
    }

    auto* app = new Application();

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
    m_Renderer->GetActivePipeline()->CreateUniformBuffer("MatricesBlock",{
    BufferElementType::FLOAT4X4, // mat4 viewProjection
    BufferElementType::FLOAT4X4}); // mat4 lightSpaceMatrix

    m_Renderer->GetActivePipeline()->CreateUniformBuffer("LightBlock",{
    BufferElementType::BOOL,
    BufferElementType::INT,
    BufferElementType::FLOAT3,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::STRUCT_END, // Struct DirectionalLight with Size-Padding to 2 Vec4
    // Array of 32 Struct PointLight with Size-Padding to 2 Vec4
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END,
    BufferElementType::STRUCT_START, BufferElementType::FLOAT3, BufferElementType::FLOAT3, BufferElementType::INT, BufferElementType::STRUCT_END});

    m_Renderer->GetActivePipeline()->CreateUniformBuffer("SettingsBlock",{
    BufferElementType::BOOL,
    BufferElementType::BOOL});

    for (const auto& renderPass : m_Renderer->GetActivePipeline()->GetRenderPasses())
    {
        renderPass->AddUsedUniformBuffer("MatricesBlock", m_Renderer->GetActivePipeline()->GetUniformBuffer("MatricesBlock"));
        renderPass->AddUsedUniformBuffer("LightBlock",m_Renderer->GetActivePipeline()->GetUniformBuffer("LightBlock"));
        renderPass->AddUsedUniformBuffer("SettingsBlock",m_Renderer->GetActivePipeline()->GetUniformBuffer("SettingsBlock"));
    }
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
