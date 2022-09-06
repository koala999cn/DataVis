#pragma once
#include <memory>
#include <vector>
#include "KvRenderable.h"
#include "KvCoordSystem.h"
#include "KvPlottable.h"


// 绘图窗口抽象类

class KvPlotWidget : public KvRenderable
{
public:
	using widget_t = void*;

	virtual widget_t* widget() const = 0; // 返回与KvPlotWidget挂接的win对象指针

	// 更新绘图场景，包括geometry和effect
	virtual void update(bool immediately = true) = 0;

private:
	std::unique_ptr<KvCoordSystem> coord_;
	std::vector<std::unique_ptr<KvPlottable>> plottables_;
};
