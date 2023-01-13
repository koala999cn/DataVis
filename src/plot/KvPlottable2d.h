#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"

class KvSampled;

// imageͼ�Ļ��࣬��Ҫ����grid��sampled2d�����ݣ����ڻ�����ͼheatmap������ͼsurface��
// ����2d��3dģʽ

class KvPlottable2d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	bool showBorder() const { return showBorder_; }
	bool& showBorder() { return showBorder_; }

	const KpPen& borderPen() const { return borderPen_; }
	KpPen& borderPen() { return borderPen_; }

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	using point_getter2 = typename KvPaint::point_getter2;
	virtual void drawImpl_(KvPaint*, point_getter2, unsigned nx, unsigned ny, unsigned channels) const = 0;

private:

	// useDefaultZΪtrueʱ���ã� �˻�Ϊ��x-yƽ�����colormapͼ��ʹ��ȱʡzֵ��
	// ʹ�����ά�ȵ�����ֵ������ɫ��ֵ
	void draw1d_(KvPaint*, KvSampled*) const;

	// useDefaultZΪfalseʱ���ã� ��3ά�ռ����colormapͼ
	// ʹ�����ά�ȵ�����ֵ������ɫ��ֵ
	void draw2d_(KvPaint*, KvSampled*) const;

private:

	bool useDefaultZ_{ false };

	bool showBorder_{ false };
	KpPen borderPen_;
};
