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
	: m_Width(width), m_Height(height), m_Title(title), m_Window(nullptr), m_SelectedObject(-1),
	m_MainFramebuffer(nullptr), m_CameraControllerFirstPerson(nullptr), m_CameraControllerArcball(nullptr),
	m_IsFocused(false), m_FirstMouse(true), m_ArcballMove(false), m_DeltaTime(0.0f), m_LastFrame(0.0f), m_RenderWindowHovered(false), m_FirstRender(true)
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

	glfwSetWindowUserPointer(m_Window, this);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
        SPDLOG_DEBUG("Failed to initialize GLAD");
	}
	glfwSwapInterval(0); // Disable vsync
	glViewport(0, 0, m_Width, m_Height);

	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		Window* thisClass = (Window*)glfwGetWindowUserPointer(window);
		thisClass->keyCallback(window, key, scancode, action, mods);
	});
	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
		Window* thisClass = (Window*)glfwGetWindowUserPointer(window);
		thisClass->cursorPosCallback(window, xpos, ypos);
	});
	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
		Window* thisClass = (Window*)glfwGetWindowUserPointer(window);
		thisClass->mouseButtonCallback(window, button, action, mods);
	});
	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
		Window* thisClass = (Window*)glfwGetWindowUserPointer(window);
		thisClass->scrollCallback(window, xoffset, yoffset);
	});

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
	float currentFrame = glfwGetTime();
	m_DeltaTime = currentFrame - m_LastFrame;
	m_LastFrame = currentFrame;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::RenderImGui(Ref<Scene> scene)
{
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Renderer", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Recompile Shaders", NULL, false) && m_CommandHandlerCallback)
				m_CommandHandlerCallback(WindowCommandEvent(WindowCommand::RecompileShaders));
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	//Setup Dockspace default layout
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id);
	if (m_FirstRender)
	{
		ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
		ImGui::DockBuilderAddNode(dockspace_id); // Add empty node
		ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetCurrentWindow()->Viewport->WorkSize);

		ImGuiID dock_main_id = dockspace_id;
		ImGuiID dock_id_scene = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
		ImGuiID dock_id_props = ImGui::DockBuilderSplitNode(dock_id_scene, ImGuiDir_Down, 0.40f, NULL, &dock_id_scene);
		ImGui::DockBuilderGetNode(dock_main_id)->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
		ImGui::DockBuilderGetNode(dock_id_scene)->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
		ImGui::DockBuilderGetNode(dock_id_props)->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;

		ImGui::DockBuilderDockWindow("Render", dock_main_id);
		ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_id_scene);
		ImGui::DockBuilderDockWindow("Properties", dock_id_props);
		ImGui::DockBuilderFinish(dockspace_id);

		m_FirstRender = false;
	}

	BuildSceneHierarchy(scene, m_SelectedObject);
	BuildProperties(m_SelectedObject);
	m_RenderWindowHovered = BuildRenderWindow(this);

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

void Window::CreateCameraController(Camera* camera)
{
	m_CameraControllerFirstPerson = CreateScope<CameraControllerFirstPerson>(camera, 3.0f, 0.1f);
	m_CameraControllerArcball = CreateScope<CameraControllerArcball>(camera, 5.0f, 0.3f);
}

void Window::ProcessInput()
{
    if (!m_CameraControllerFirstPerson || !m_IsFocused)
		return;

	if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
		m_CameraControllerFirstPerson->ProcessKeyboard(FORWARD, m_DeltaTime);
	if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
		m_CameraControllerFirstPerson->ProcessKeyboard(BACKWARD, m_DeltaTime);
	if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
		m_CameraControllerFirstPerson->ProcessKeyboard(LEFT, m_DeltaTime);
	if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
		m_CameraControllerFirstPerson->ProcessKeyboard(RIGHT, m_DeltaTime);
	if (glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !m_Sprinting)
	{
		m_CameraControllerFirstPerson->SetSpeed(8.0f);
		m_Sprinting = true;
	}
	else if (glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE && m_Sprinting)
	{
		m_CameraControllerFirstPerson->SetSpeed(3.0f);
		m_Sprinting = false;
	}
}

void Window::SetCommandHandler(WindowCommandEventCallbackFn commandHandlerCallback)
{
	m_CommandHandlerCallback = commandHandlerCallback;
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		m_IsFocused = !m_IsFocused;
		if (m_IsFocused)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_FirstMouse = true;
	}

}

void Window::cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
	float xpos = static_cast<float>(xPos);
	float ypos = static_cast<float>(yPos);

	if (m_FirstMouse)
	{
		m_LastX = xpos;
		m_LastY = ypos;
		m_FirstMouse = false;
	}

	float xOffset = xpos - m_LastX;
	float yOffset = m_LastY - ypos;
	m_LastX = xpos;
	m_LastY = ypos;

	if (m_CameraControllerFirstPerson && m_IsFocused)
	{
		m_CameraControllerFirstPerson->ProcessMouseMovement(xOffset, yOffset);
	}
	else if (m_CameraControllerArcball && m_ArcballMove)
	{
		m_CameraControllerArcball->ProcessMouseMovement(xOffset, yOffset);
	}
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (!m_RenderWindowHovered)
		return;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
			m_ArcballMove = true;
		else if (action == GLFW_RELEASE)
			m_ArcballMove = false;
	}
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (m_CameraControllerArcball && m_RenderWindowHovered)
		m_CameraControllerArcball->ProcessScroll(yoffset);
}
