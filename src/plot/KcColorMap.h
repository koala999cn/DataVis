#pragma once
#include "KvPlottable.h"
#include "KtGradient.h"


// ºÊ»›∆◊Õº∫Õ»»Õº

class KcColorMap : public KvPlottable
{
	using super_ = KvPlottable;

public:

	KcColorMap(const std::string_view& name);

	unsigned majorColorsNeeded() const override;

	bool minorColorNeeded() const override;

	unsigned majorColors() const override;

	color4f majorColor(unsigned idx) const override;

	void setMajorColors(const std::vector<color4f>& majors) override;

	color4f minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	aabb_t boundingBox() const override;

	float mapLower() const { return mapLower_; }
	float& mapLower() { return mapLower_; }

	float mapUpper() const { return mapUpper_; }
	float& mapUpper() { return mapUpper_; }

	bool showText() const { return showText_; }
	bool& showText() { return showText_; }

	color4f textColor() const { return clrText_; }
	color4f& textColor() { return clrText_; }

	bool showBorder() const { return showBorder_; }
	bool& showBorder() { return showBorder_; }

	const KpPen& borderPen() const { return borderPen_; }
	KpPen& borderPen() { return borderPen_; }

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	color4f mapValueToColor_(float_t val) const;

private:
	float mapLower_{ 0 }, mapUpper_{ 1 }; // color mapµƒ÷µ”Ú∑∂Œß

	KtGradient<float_t, color4f> mapper_;

	bool showBorder_{ false };
	KpPen borderPen_;

	bool showText_{ false };
	color4f clrText_{ 1, 0, 0, 1 };
};
