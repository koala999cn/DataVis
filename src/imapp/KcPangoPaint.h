#pragma once
#include "KcImOglPaint.h"


// 实现opengl环境下基于pango的文本绘制

class KcPangoPaint : public KcImOglPaint
{
	using super_ = KcImOglPaint;

public:

	KcPangoPaint();
	~KcPangoPaint();

	point2 textSize(const std::string_view& text) const override;

	void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const std::string_view& text) override;

private:

	void setFont_() const;

	void expandSurf_(); // 拓展cairoSurf_尺寸
	void expandNodes_(); // 拓展rpnodes_

private:
	void* cairoCxt_{ nullptr };
	void* cairoSurf_{ nullptr }; // the surface of cairo
	void* pangoLayout_{ nullptr };
	void* rectpack_{ nullptr };
	void* rpnodes_{ nullptr };
	unsigned nnodes_; // # of rpnodes_
	unsigned nused_; // # of rpnodes_ used
};
