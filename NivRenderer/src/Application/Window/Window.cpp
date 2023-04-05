#include "Window.h"

#include "Entity/ECSRegistry.h"
#include "Application/Window/SceneHierarchy.h"
#include "Application/Window/Properties.h"
#include "Application/Window/RenderWindow.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

Window::Window(uint32_t width, uint32_t height, const char* title)
	: m_Width(width), m_Height(height), m_Title(title), m_Window(nullptr), m_SelectedObject(-1), m_MainFramebuffer(nullptr)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	if (!glfwInit())
		return;

	m_Window = glfwCreateWindow(width, height, title, NULL, NULL);
}

Window::~Window()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Window::CreateRenderContext()
{
	glfwMakeContextCurrent(m_Window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
	}
	glfwSwapInterval(0); // Disable vsync
	glViewport(0, 0, m_Width, m_Height);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
	ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_DockingEnable;
	m_MainFramebuffer = CreateScope<Framebuffer>(GetWidth(), GetHeight());
}

bool Window::ShouldClose()
{
	if(m_Window)
		return glfwWindowShouldClose(m_Window);
	return false;
}

void Window::PrepareFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::RenderImGui(Ref<Scene> scene)
{
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Renderer", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id);
	BuildSceneHierarchy(scene, m_SelectedObject);
	BuildProperties(m_SelectedObject);
	BuildRenderWindow(this);
	ImGui::End();
	ImGui::Render();
}

void Window::PollEvents()
{
	if(m_Window)
		glfwPollEvents();
}

void Window::SwapBuffers()
{
	if (m_Window)
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(m_Window);
	}
}

void Window::UpdateFramebuffer(uint32_t width, uint32_t height)
{
	if (m_MainFramebuffer->GetWidth() != width || m_MainFramebuffer->GetHeight() != height)
	{
		m_MainFramebuffer = nullptr;
		m_MainFramebuffer = CreateScope<Framebuffer>(width, height);
		glViewport(0, 0, width, height);
	}
}
