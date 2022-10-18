#pragma once
#include "imgui.h"


// imguiµÄÀ©Õ¹º¯Êý

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
	return { a.x + b.x, a.y + b.y };
}

inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) {
	return { a.x - b.x, a.y - b.y };
}

inline ImVec2 operator*(const ImVec2& a, float f) {
	return { a.x * f, a.y * f };
}

inline ImVec2 operator/(const ImVec2& a, float f) {
	return { a.x / f, a.y / f };
}


namespace ImGuiX
{
	// reference https://github.com/ocornut/imgui/pull/1118
	IMGUI_API void  AddLineDashed(ImDrawList* dl, const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f, unsigned int segments = 10, unsigned int on_segments = 1, unsigned int off_segments = 1);
}