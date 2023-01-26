#pragma once
#include "imgui.h"
#include <vector>
#include <functional>
#include "../plot/KtColor.h"
#include "../plot/KtMargins.h"

class KvData;
class KpPen;
class KpBrush;
class KpMarker;
class KeAlignment;

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
	void addLineDashed(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f, unsigned int segments = 10, unsigned int on_segments = 1, unsigned int off_segments = 1);

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

	// 显示巨型表格的通用函数
	void showLargeTable(unsigned rows, unsigned cols, std::function<void(unsigned, unsigned)> fnShow, 
		unsigned freezeCols = 1, unsigned freeszRows = 1, const std::vector<std::string>& headers = {});

	bool alignment(const char* label, KeAlignment& align, bool defaultOpen);

	bool treePush(const char* label, bool defaultOpen);

	void treePop();

	// treenode + checkbox + label
	bool cbTreePush(const char* label, bool* show, bool* open);

	void cbTreePop();

	// treenode + checkbox + text-editor + label
	bool cbiTreePush(const char* label, bool* show, std::string* text, bool* open);

	void cbiTreePop();

	bool cbInputText(const char* label, bool* show, std::string* text);

	bool penStyle(const char* label, int& style);

	bool pen(KpPen& cxt, bool showStyle);

	bool brush(KpBrush& cxt, bool showStyle);

	bool markerType(const char* label, int& type);

	bool marker(KpMarker& cxt);

	bool margins(const char* label, KtMargins<float>& m);

	bool margins(const char* label, KtMargins<double>& m);

	template<int N>
	bool combo(const char* label, const char*(&enums)[N], int& val) {
		bool select_changed = false;
		if (ImGui::BeginCombo(label, enums[val])) {
			for (unsigned i = 0; i < N; i++) {
				if (ImGui::Selectable(enums[i], i == val)) 
					val = i, select_changed = true;
				if (i == val) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		return select_changed;
	}
}
