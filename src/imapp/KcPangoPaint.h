#pragma once
#include "KcImOglPaint.h"
#include <string>
#include <memory>

class KvCairoSurface;

// 实现opengl环境下基于pango的文本绘制
// 主要思路：创建一张与画布同等大小的cairo-image作为绘制文本的基底（beginPaint实现），
// 然后使用pango依次绘制本文（drawText成员方法实现），最后将cairo-image贴到opengl画布上（endPaint实现）

class KcPangoPaint : public KcImOglPaint
{
	using super_ = KcImOglPaint;

public:

	KcPangoPaint();
	~KcPangoPaint();

	void beginPaint() override;
	void drawRenderList_() override;

	point2 textSize(const std::string_view& text) const override;

	void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const std::string_view& text) override;

private:
	void setFont_() const;

private:
	std::unique_ptr<KvCairoSurface> cairoSurf_;
	void* pangoLayout_{ nullptr };
	std::string fontDesc_; // 临时变量，防止重复设置相同的字体
};
