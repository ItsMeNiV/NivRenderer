﻿#include "NivRenderer.h"

#include "Entity/ECSRegistry.h"
#include "Entity/Entities/SceneObject.h"
#include "Entity/Components/TransformComponent.h"
#include "Rendering/ForwardPipeline/ForwardPass.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

int main()
{
	Application* app = new Application();

	app->Run();

	delete app;
}

Application::Application()
	: m_Window(CreateRef<Window>(1600, 900, "NivRenderer")), m_Scene(CreateRef<Scene>())
{
	m_Window->SetCommandHandler([&](WindowCommandEvent command) { handleWindowCommand(command); });

	m_Renderer = CreateRef<Renderer>(m_Window);

	setupDefaultScene();

	Ref<RenderPass> forwardPass = CreateRef<ForwardPass>(CreateRef<Shader>("assets/shaders/forwardrender.glsl", ShaderType::VERTEX_AND_FRAGMENT), 1920, 1080);
	std::vector<Ref<RenderPass>> renderPasses;
	renderPasses.push_back(forwardPass);

	m_Renderer->SetActivePipeline(CreateRef<RenderPipeline>(renderPasses, std::vector<Ref<RenderPass>>(), 1920, 1080, 4));
	m_Renderer->SetActiveScene(m_Scene);
}

void Application::Run()
{
	while (!m_Window->ShouldClose())
	{
		m_Window->PollEvents();
		m_Window->PrepareFrame();

		m_Window->RenderImGui(m_Scene);

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
}

void Application::setupDefaultScene()
{
	m_Scene->AddSceneObject();
	m_Scene->AddSceneDirectionalLight();
	Ref<Camera> camera(CreateRef<Camera>(glm::vec3(0.0f, 0.0f, 5.0f), 1920, 1080));
	m_Scene->AddCamera(camera);
	m_Window->CreateCameraController(camera.get());
}
