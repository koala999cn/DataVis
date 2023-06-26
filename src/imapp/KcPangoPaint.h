#pragma once
#include "KcImOglPaint.h"
#include <string>
#include <memory>

class KvCairoSurface;

// ʵ��opengl�����»���pango���ı�����
// ��Ҫ˼·������һ���뻭��ͬ�ȴ�С��cairo-image��Ϊ�����ı��Ļ��ף�beginPaintʵ�֣���
// Ȼ��ʹ��pango���λ��Ʊ��ģ�drawText��Ա����ʵ�֣������cairo-image����opengl�����ϣ�endPaintʵ�֣�

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
	std::string fontDesc_; // ��ʱ��������ֹ�ظ�������ͬ������
};
