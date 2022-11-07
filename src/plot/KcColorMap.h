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

	void draw(KvPaint*) const override;

	float_t valueLower() const { return valLower_; }
	float_t& valueLower() { return valLower_; }

	float_t valueUpper() const { return valUpper_; }
	float_t& valueUpper() { return valUpper_; }

protected:

	color4f mapValueToColor_(float_t val) const;

private:
	float_t valLower_{ 0 }, valUpper_{ 1 }; // color mapµÄÖµÓò·¶Î§

	KtGradient<float_t, color4f> mapper_;
	color4f border_{ 0 };
};
