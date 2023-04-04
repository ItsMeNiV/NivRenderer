#include "Window.h"

Window::Window(uint32_t width, uint32_t height, const char* title)
	: m_Width(width), m_Height(height), m_Title(title), m_Window(nullptr)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	if (!glfwInit())
		return;

	m_Window = glfwCreateWindow(width, height, title, NULL, NULL);
}

Window::~Window()
{
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
}

bool Window::ShouldClose()
{
	if(m_Window)
		return glfwWindowShouldClose(m_Window);
	return false;
}

void Window::PollEvents()
{
	if(m_Window)
		glfwPollEvents();
}

void Window::SwapBuffers()
{
	if(m_Window)
		glfwSwapBuffers(m_Window);
}
