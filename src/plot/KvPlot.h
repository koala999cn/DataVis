#pragma once
#include <memory>
#include <vector>
#include "KvPlottable.h"
#include "KcCoordSystem.h"
#include "KtVector4.h"

class KvPlot
{
public:
	using vec4 = KtVector4<double>;

	KvPlot(KcCoordSystem* coord) : coord_(coord), ortho_(true) {}

	virtual void show(bool b) = 0;

	virtual bool visible() const = 0;

	virtual void* widget() const = 0;

	// 更新绘图
	virtual void update(bool immediately = true) = 0;

	virtual vec4 background() const = 0;
	virtual void setBackground(const vec4& clr) = 0;

	KcCoordSystem* coordSystem() {
		return coord_.get();
	}

	int numPlottables() const {
		return plottables_.size();
	}

	KvPlottable* plottable(int idx) {
		return plottables_[idx].get();
	}

protected:
	std::unique_ptr<KcCoordSystem> coord_;
	std::vector<std::unique_ptr<KvPlottable>> plottables_;
	bool ortho_; // 正交投影 vs. 透视投影
};
