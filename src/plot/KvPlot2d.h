#pragma once
#include "KvPlot.h"
#include "KvRenderable.h"
#include "KtMargins.h"


class KcAxis;

// 二维plot的抽象类

class KvPlot2d : public KvPlot
{
public:
	using float_t = typename KvRenderable::float_t;
	using axis_ptr = std::shared_ptr<KcAxis>;
	using point2 = point2d;

	KvPlot2d(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord);

	const KtMargins<float_t>& margins() const { return margins_; }
	KtMargins<float_t>& margins() { return margins_; }

protected:
	void autoProject_() override;

protected:
	KtMargins<float_t> margins_{ 15, 15, 15, 15 };
};
