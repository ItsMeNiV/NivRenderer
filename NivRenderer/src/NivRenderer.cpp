#include "NivRenderer.h"

#include "Rendering/ForwardPipeline/ForwardPass.h"
#include "Application/Util/Instrumentor.h"
#include "Application/Serialization/SerializationManager.h"
#include "portable-file-dialogs.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_ERROR

int main()
{
    spdlog::set_level(spdlog::level::err);

    auto* app = new Application();

	app->Run();

	delete app;
}

Application::Application() :
    m_Window(CreateScope<Window>(1600, 900, "NivRenderer"))
{
    const auto folder = pfd::select_folder("Select Project Folder", ".").result();
    std::string projectFilepath = folder + "\\\\New Project.nivproj";
    bool createDefaultScene = true;
    if (!folder.empty())
    {
        bool importProject = false;
        for (const auto& entry : std::filesystem::directory_iterator(folder))
        {
            std::string filename = entry.path().filename().string();
            std::cmatch m;
            if (std::regex_match(filename.c_str(), m, std::regex(R"(^.*.nivproj$)")))
            {
                importProject = pfd::message("Existing Project found",
                             "Already existing Project found in the selected Folder. Do you want to import it?",
                             pfd::choice::yes_no).result() == pfd::button::yes;
                projectFilepath = entry.path().string();
                break;
            }
        }

        if (importProject)
        {
            createDefaultScene = false;
            m_Project = CreateScope<Project>("");
            Project* project = SerializationManager::LoadProject(projectFilepath);
            m_Project.reset(project);
            m_Window->CreateCameraAndController(m_Project->GetActiveScene()->GetSceneSettings().renderResolution);
            m_Project->GetActiveScene()->AddCamera(m_Window->GetCamera());
        }
        else
        {
            projectFilepath = pfd::save_file("Create new Project", folder, {"NivRenderer Project", "*.nivproj"}).result();
            if (projectFilepath.empty())
                projectFilepath = folder + "\\\\New Project";

            const std::string fileEnding = projectFilepath.find_last_of('.') == std::string::npos ? "" : projectFilepath.substr(projectFilepath.find_last_of('.'), projectFilepath.size());
            if (fileEnding != ".nivproj")
                projectFilepath += ".nivproj";
            m_Project = CreateScope<Project>(projectFilepath);
        }
    }
    else
        m_Project = CreateScope<Project>("New Project");

	m_Window->SetCommandHandler([&](WindowCommandEvent command) { handleWindowCommand(command); });

	m_Renderer = CreateScope<Renderer>(m_Window.get());
    Scene* scene = m_Project->GetActiveScene();
    m_Renderer->SetScene(scene);

    if (createDefaultScene)
    {
        setupDefaultScene();
        SerializationManager::SaveProject(m_Project.get(), true);
    }

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
    if (command.GetCommand() == WindowCommand::SaveProject)
        SerializationManager::SaveProject(m_Project.get(), true);
    if (command.GetCommand() == WindowCommand::SaveProjectAs)
        SerializationManager::SaveProject(m_Project.get(), false);
    if (command.GetCommand() == WindowCommand::LoadProject)
    {
        if(Project* project = SerializationManager::LoadProject())
        {
            m_Renderer->ResetProxies();
            m_Project.reset(project);
            m_Window->CreateCameraAndController(m_Project->GetActiveScene()->GetSceneSettings().renderResolution);
            m_Project->GetActiveScene()->AddCamera(m_Window->GetCamera());
            m_Renderer->SetScene(m_Project->GetActiveScene());
        }
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
