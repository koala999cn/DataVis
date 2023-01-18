#pragma once
#include "KcScatter.h"


// 气泡图：适用于2d/3d数据. 本质上是尺寸插值
// 
//   - 对于2d数据，y既为坐标又为数据值
//   - 对于3d数据，y为坐标，z为数据值
// 

class KcBubble : public KcScatter
{
	using super_ = KcScatter;

public:

	using super_::super_;

protected:
	virtual void drawImpl_(KvPaint*, GETTER, unsigned count, unsigned channels) const final;

	float mapValueToSize_(float_t val) const;

private:

	// 根据value的大小，对bubble的尺寸进行插值
	bool radiusAsSize_{ false }; // true表示按半径插值，否则按面积插值
	float sizeLower_{ 3 }, 	sizeUpper_{ 33 }; // 尺寸插值范围
};
