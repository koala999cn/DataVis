#pragma once
#include "KvPlottable1d.h"


// 气泡图：适用于2d/3d数据. 本质上是尺寸插值
// 
//   - 对于2d数据，y既为坐标又为数据值
//   - 对于3d数据，y为坐标，z为数据值
// 

class KcBubble : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	const color4f& minorColor() const override;
	void setMinorColor_(const color4f& minor) override;

	bool radiusAsSize() const { return radiusAsSize_; }
	bool& radiusAsSize() { return radiusAsSize_; }

	float sizeLower() const { return sizeLower_; }
	float& sizeLower() { return sizeLower_; }

	float sizeUpper() const { return sizeUpper_; }
	float& sizeUpper() { return sizeUpper_; }

	bool showText() const { return showText_; }
	bool& showText() { return showText_; }

	color4f textColor() const { return clrText_; }
	color4f& textColor() { return clrText_; }

protected:

	bool showFill_() const final;

	bool showEdge_() const final;

	void setRenderState_(KvPaint*, unsigned objIdx) const final;

	void* drawObjectImpl_(KvPaint*, GETTER, unsigned count, unsigned objIdx) const final;

	float mapValueToSize_(float_t val) const;

private:

	// 根据value的大小，对bubble的尺寸进行插值
	bool radiusAsSize_{ false }; // true表示按半径插值，否则按面积插值
	float sizeLower_{ 3 }, 	sizeUpper_{ 33 }; // 尺寸插值范围

	color4f clrMinor_{ 0, 0, 0, 1 }; // 辅色，用于绘制渐变色

	bool showText_{ true };
	color4f clrText_{ 1, 0, 0, 1 };
};
