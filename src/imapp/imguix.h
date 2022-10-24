#pragma once
#include "imgui.h"
#include <vector>
#include <functional>
#include "../plot/KtColor.h"

class KvData;

// imgui的扩展函数

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
	void AddLineDashed(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f, unsigned int segments = 10, unsigned int on_segments = 1, unsigned int off_segments = 1);

	// @startPos: 绘制的起始位置
	// @blockSize: 每个色块的尺寸
	// @blockSpace: 色块之间的留空
	void drawColorBar(std::vector<color4f>& colors, const ImVec2& startPos, const ImVec2& blockSize, float blockSpace);

	void showDataTable(const KvData& data);

	using matrixd = std::vector<std::vector<double>>;

	// @type: the type of data, see KuDataUtil::KeDataType for detail
	void showDataTable(int type, const matrixd& data, bool rowMajor = false);

	// @type: the type of data, see KuDataUtil::KeDataType for detail
	void showDataTable(int type, unsigned rows, unsigned cols, std::function<double(unsigned, unsigned)> fn);

	bool prefixCheckbox(const char* label, bool* v);
}
