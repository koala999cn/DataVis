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

	using KvPlot::KvPlot;

	const KtMargins<float_t>& margins() const { return margins_; }
	KtMargins<float_t>& margins() { return margins_; }

	virtual mat4 projMatrix() const = 0;
	virtual void setProjMatrix(const mat4&) = 0;

private:
	void autoProject_() override;

protected:
	KtMargins<float_t> margins_{ 15, 15, 15, 15 };
};
