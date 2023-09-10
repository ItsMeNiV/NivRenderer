#pragma once
#include "Base.h"

#include "Application/Window/CameraControllerArcball.h"
#include "Application/Scene.h"
#include "Rendering/OpenGL/Framebuffer.h"
#include "Application/Window/WindowEvents.h"

class Window
{
public:
	using WindowCommandEventCallbackFn = std::function<void(WindowCommandEvent)>;

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
	void SetCommandHandler(WindowCommandEventCallbackFn commandHandlerCallback);

	const uint32_t GetWidth() const { return m_Width; }
	const uint32_t GetHeight() const { return m_Height; }
	Framebuffer* const GetFramebuffer() const { return m_MainFramebuffer.get(); }
    double GetWindowRuntime() const { return glfwGetTime(); }

private:
	GLFWwindow* m_Window;
	uint32_t m_Width, m_Height;
	const char* m_Title;
	float m_DeltaTime, m_LastFrame;
	WindowCommandEventCallbackFn m_CommandHandlerCallback;
	bool m_FirstRender;

	//Scene-Hierarchy
	int32_t m_SelectedObject;

	//Render-Window
	Scope<Framebuffer> m_MainFramebuffer;
	bool m_RenderWindowHovered;

	//Camera Controller
	Scope<CameraControllerFirstPerson> m_CameraControllerFirstPerson;
	Scope<CameraControllerArcball> m_CameraControllerArcball;
	bool m_FirstMouse, m_Sprinting, m_IsFocused, m_ArcballMove;
	float m_LastX, m_LastY;

	//Gizmo
    int32_t m_GizmoType;

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};