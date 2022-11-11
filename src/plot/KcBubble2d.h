#pragma once
#include "KcColorMap.h"


// 气泡图：适用于2d/3d数据
//   - 对于2d数据，y既为坐标又为数据值
//   - 对于3d数据，y为坐标，z为数据值

class KcBubble2d : public KcColorMap
{
	using super_ = KcColorMap;

public:

	using super_::super_;

	unsigned majorColorsNeeded() const override;

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	float mapValueToSize_(float_t val) const;

private:

	// 根据value的大小，对bubble的大小进行插值
	bool radiusAsSize_{ false }; // true表示按半径插值，否则按面积插值
	float sizeLower_{ 3 }, 	sizeUpper_{ 33 }; // 插值范围

	bool enableSizeInterp_{ true }; // 启用大小插值
 	bool enableColorInterp_{ false }; // 启用颜色插值
};
