#pragma once
#include "imgui.h"
#include <vector>
#include <functional>
#include "KtAABB.h"
#include "../plot/KtColor.h"
#include "../plot/KtGradient.h"
#include "../layout/KvLayoutElement.h"

class KvData;
class KpPen;
class KpBrush;
class KpMarker;
class KpLabel;
class KpNumericFormatter;
class KeAlignment;

// imgui����չ����

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

	// @startPos: ���Ƶ���ʼλ��
	// @blockSize: ÿ��ɫ��ĳߴ�
	// @blockSpace: ɫ��֮�������
	void drawColorBar(std::vector<color4f>& colors, const ImVec2& startPos, const ImVec2& blockSize, float blockSpace);

	void showDataTable(const char* label, const KvData& data);

	void showDataTable(const char* label, const KvData& data, std::vector<char>& vis);

	using matrixd = std::vector<std::vector<double>>;

	// @type: the type of data, see KuDataUtil::KeDataType for detail
	void showDataTable(const char* label, int type, const matrixd& data, bool rowMajor, std::vector<char>& vis);

	// @type: the type of data, see KuDataUtil::KeDataType for detail
	void showDataTable(const char* label, int type, unsigned rows, unsigned cols, 
		std::function<double(unsigned, unsigned)> fn, std::vector<char>& vis);

	// ��ʾ���ͱ���ͨ�ú���
	void showLargeTable(const char* label, unsigned rows, unsigned cols, 
		std::function<void(unsigned, unsigned)> fnShow,
		unsigned freezeCols = 1, unsigned freeszRows = 1, 
		const std::vector<std::string>& headers = {},
		char* vis = nullptr);

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

	bool pen(KpPen& cxt, bool showStyle, bool showColor);

	bool brush(KpBrush& cxt, bool showStyle);

	bool markerType(const char* label, int& type);

	bool marker(KpMarker& cxt);

	bool label(KpLabel& l);

	bool format(KpNumericFormatter& f);

	bool margins(const char* label, KtAABB<float, 2>& m);

	bool margins(const char* label, KtAABB<double, 2>& m);

	// gradient editor
	bool gradient(const char* label, KtGradient<float, color4f>& grad, float& selectedKey);

	// ��ʾһ����ɫ�༭�������ڱ༭����ɫ
	// @count: ��ɫ����
	// @fullfill: ��true��������item-width��ȣ�����ColorEdit��ť��С˳������
	bool multiColorsEdit(const char* label, ImVec4* clrs, int count, bool fullfill);

	// ���ʽ�༭���
	// @dim: ά�����ƣ�0��ʾ������
	// @handler: �༭ȷ�Ϻ�Ļص�����
	void exprEdit(const char* label, const char* text, unsigned dim,
		std::function<void(std::shared_ptr<KvData>, const char*)> handler);

	void layout(const char* label, KvLayoutElement* lay);
}
