#pragma once
#include "KvPlottable.h"
#include "KtGradient.h"


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

	aabb_type boundingBox() const override;

	float valueLower() const { return valLower_; }
	float& valueLower() { return valLower_; }

	float valueUpper() const { return valUpper_; }
	float& valueUpper() { return valUpper_; }

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	color4f mapValueToColor_(float_t val) const;

private:
	float valLower_{ 0 }, valUpper_{ 1 }; // color mapµÄÖµÓò·¶Î§

	KtGradient<float_t, color4f> mapper_;
	color4f border_{ 0 };
};
