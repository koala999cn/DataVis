#pragma once
#include "KcImPaint.h"
#include <vector>
#include <memory>
#include <functional>
#include "opengl/KcRenderObject.h"


// ����ImGui������OpenGL��KvPaintʵ��

class KcImOglPaint : public KcImPaint
{
	using super_ = KcImPaint;

public:

	struct TextVbo
	{
		point3f pos;
		point2f uv;
		color4f clr;
	};

	using super_::super_;

	void beginPaint() override;
	void endPaint() override;

	void drawPoint(const point3& pt) override;

	void drawPoints(point_getter fn, unsigned count) override;

	void drawLine(const point3& from, const point3& to) override;

	void drawLineStrip(point_getter fn, unsigned count) override;

	void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text) override;

private:

	point3 toNdc_(const point3& pt) const;

private:
	std::vector<std::unique_ptr<KcRenderObject>> objs_; // ����plottables
	std::vector<std::function<void(void)>> fns_; // ������������
	std::vector<TextVbo> texts_; // �����ı�
};
