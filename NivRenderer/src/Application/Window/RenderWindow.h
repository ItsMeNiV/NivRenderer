#pragma once
#include "imgui.h"

void BuildRenderWindow(Window* window)
{
	ImGui::Begin("Render", 0, ImGuiWindowFlags_NoCollapse);
	window->UpdateFramebuffer(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
	if (window->GetFramebuffer())
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		unsigned int framebuffer = window->GetFramebuffer()->GetId();
		ImVec2 maxPos(ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth(), ImGui::GetCursorScreenPos().y + ImGui::GetWindowHeight());

		drawList->AddImage((void*)framebuffer, ImGui::GetCursorScreenPos(), maxPos);
	}
	ImGui::End();
}