#pragma once
#include "imgui.h"

void BuildRenderWindow(Window* window)
{
	ImGui::Begin("Render", 0, ImGuiWindowFlags_NoCollapse);
	window->UpdateFramebuffer(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	if (window->GetFramebuffer())
	{
		ImGui::BeginChild("Frame");
		ImVec2 wsize = ImGui::GetWindowViewport()->Size;
		unsigned int framebufferTexture = window->GetFramebuffer()->GetTextureColorBuffer()->GetTextureId();
		ImGui::Image((ImTextureID)framebufferTexture, wsize, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndChild();
	}
	ImGui::End();
}