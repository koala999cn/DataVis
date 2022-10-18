#include "imguix.h"


namespace ImGuiX
{
	IMGUI_API void  AddLineDashed(ImDrawList* dl, const ImVec2& a, const ImVec2& b, ImU32 col, float thickness, unsigned int num_segments, unsigned int on_segments, unsigned int off_segments)
	{
        if ((col >> 24) == 0)
            return;
        int on = 0, off = 0;
        ImVec2 dir = (b - a) / num_segments;
        for (int i = 0; i <= num_segments; i++)
        {
            ImVec2 point(a + dir * i);
            if (on < on_segments) {
                dl->_Path.push_back(point);
                on++;
            }
            else if (on == on_segments && off == 0) {
                dl->_Path.push_back(point);
                dl->PathStroke(col, false, thickness);
                off++;
            }
            else if (on == on_segments && off < off_segments) {
                off++;
            }
            else {
                dl->_Path.resize(0);
                dl->_Path.push_back(point);
                on = 1;
                off = 0;
            }
        }
        dl->PathStroke(col, false, thickness);
	}
}