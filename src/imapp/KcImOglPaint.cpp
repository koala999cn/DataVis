#include "KcImOglPaint.h"
#include "imgui.h"
#include "glad.h"


void oglDrawLineStrip(const ImDrawList* parent_list, const ImDrawCmd* cmd)
{
	cmd->UserCallbackData;
	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void KcImOglPaint::drawLineStrip(point_getter fn, unsigned count)
{
	auto dl = ImGui::GetWindowDrawList();
	dl->AddCallback(oglDrawLineStrip, nullptr);
}
