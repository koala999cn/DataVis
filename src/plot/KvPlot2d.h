#pragma once
#include "KvPlot.h"
#include "KvRenderable.h"
#include "KtMatrix4.h"
#include "KtMargins.h"


class KcAxis;

// 二维plot的抽象类

class KvPlot2d : public KvPlot
{
public:
	using float_t = typename KvRenderable::float_t;
	using axis_ptr = std::shared_ptr<KcAxis>;
	using point2 = point2d;
	using mat4 = KtMatrix4<float_t>;

	KvPlot2d(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord);

	virtual mat4 projMatrix() const = 0;
	virtual void setProjMatrix(const mat4&) = 0;

private:
	void autoProject_() override;
};
