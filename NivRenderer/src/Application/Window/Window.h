#pragma once
#include "Base.h"

#include "Application/Scene.h"
#include "Rendering/OpenGL/Framebuffer.h"

class Window
{
public:
	Window(uint32_t width, uint32_t height, const char* title);
	~Window();

	void CreateRenderContext();
	bool ShouldClose();
	void PrepareFrame();
	void RenderImGui(Ref<Scene> scene);
	void PollEvents();
	void SwapBuffers();
	void UpdateFramebuffer(uint32_t width, uint32_t height);

	const uint32_t GetWidth() const { return m_Width; }
	const uint32_t GetHeight() const { return m_Height; }
	Framebuffer* const GetFramebuffer() const { return m_MainFramebuffer.get(); }

private:
	GLFWwindow* m_Window;
	uint32_t m_Width, m_Height;
	const char* m_Title;

	//Scene-Hierarchy
	int32_t m_SelectedObject;

	//Render-Window
	Scope<Framebuffer> m_MainFramebuffer;
};