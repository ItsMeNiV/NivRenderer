#pragma once
#include "Base.h"

class Window
{
public:
	Window(uint32_t width, uint32_t height, const char* title);
	~Window();

	void CreateRenderContext();
	bool ShouldClose();
	void PollEvents();
	void SwapBuffers();

private:
	GLFWwindow* m_Window;
	int m_Width, m_Height;
	const char* m_Title;
};