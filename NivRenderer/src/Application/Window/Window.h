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
	void CreateCameraController(Camera* camera);
	void ProcessInput();

	const uint32_t GetWidth() const { return m_Width; }
	const uint32_t GetHeight() const { return m_Height; }
	Framebuffer* const GetFramebuffer() const { return m_MainFramebuffer.get(); }

private:
	GLFWwindow* m_Window;
	uint32_t m_Width, m_Height;
	const char* m_Title;
	bool m_IsFocused;
	float m_DeltaTime, m_LastFrame;

	//Scene-Hierarchy
	int32_t m_SelectedObject;

	//Render-Window
	Scope<Framebuffer> m_MainFramebuffer;

	//Camera Controller
	Scope<CameraControllerFirstPerson> m_CameraController;
	bool m_FirstMouse, m_Sprinting;
	float m_LastX, m_LastY;

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
};