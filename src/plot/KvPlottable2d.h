#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"
#include "KtGradient.h"

class KvSampled;

// imageͼ�Ļ��࣬��Ҫ����grid��sampled2d�����ݣ����ڻ�����ͼheatmap������ͼsurface��
// ����2d��3dģʽ

class KvPlottable2d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	KvPlottable2d(const std::string_view& name);

	unsigned majorColorsNeeded() const override;

	unsigned majorColors() const override;

	color4f majorColor(unsigned idx) const override;

	void setMajorColors(const std::vector<color4f>& majors) override;

	bool minorColorNeeded() const override;

	color4f minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	float mapLower() const { return mapLower_; }
	float& mapLower() { return mapLower_; }

	float mapUpper() const { return mapUpper_; }
	float& mapUpper() { return mapUpper_; }

	bool showBorder() const { return showBorder_; }
	bool& showBorder() { return showBorder_; }

	const KpPen& borderPen() const { return borderPen_; }
	KpPen& borderPen() { return borderPen_; }

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	using point_getter2 = typename KvPaint::point_getter2;
	virtual void drawImpl_(KvPaint*, point_getter2, unsigned nx, unsigned ny, unsigned channels) const = 0;

	virtual color4f mapValueToColor_(float_t val) const;

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

	float mapLower_{ 0 }, mapUpper_{ 1 }; // color map��ֵ��Χ

	KtGradient<float_t, color4f> mapper_;
};
