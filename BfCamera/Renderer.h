#pragma once

#include "ImGui/imgui.h"
#include <string>

class Renderer
{
public:
	void Initialize();

	void BeginScene();
	void DrawScene();
	void EndScene();

	static Renderer* GetInstance();

private:
	Renderer();
	~Renderer();

public:
	float DrawString(ImFont* pFont, const std::string& text, const ImVec2& position, float size, uint32_t color, bool center = false);
	void DrawLine(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness = 1.0f);
	void DrawCircle(const ImVec2& position, float radius, uint32_t color, float thickness = 1.0f);
	void DrawCircleFilled(const ImVec2& position, float radius, uint32_t color);
private:
	ImFont* m_pFont;

	static Renderer* m_pInstance;
};