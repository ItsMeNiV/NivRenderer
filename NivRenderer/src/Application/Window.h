#pragma once
#include "Base.h"

#include "Application/Scene.h"

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

private:
	GLFWwindow* m_Window;
	int m_Width, m_Height;
	const char* m_Title;

	void displaySceneObject(Ref<Scene> scene, Ref<SceneObject> sceneObject, int32_t& selectedItem);
};