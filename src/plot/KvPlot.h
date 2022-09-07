#pragma once
#include <memory>
#include <vector>
#include <QColor>
#include "KvPlottable.h"
#include "KvCoordSystem.h"

class KvPlot
{
public:

	KvPlot(KvCoordSystem* coord) : coord_(coord) {}

	virtual void show(bool b) = 0;

	virtual bool visible() const = 0;

	virtual void* widget() const = 0;

	// 告知KvPlot绘图数据是比较稳定（静态数据）还是更新频繁（动态数据），以便优化显示
	virtual void setImmutable(bool b) = 0;

	// 更新绘图
	virtual void update(bool immediately = true) = 0;

	virtual QColor background() const = 0;
	virtual void setBackground(const QColor& clr) = 0;

	KvCoordSystem* coordSystem() {
		return coord_.get();
	}

	int numPlottables() const {
		return plottables_.size();
	}

	KvPlottable* plottable(int idx) {
		return plottables_[idx].get();
	}

protected:
	std::unique_ptr<KvCoordSystem> coord_;
	std::vector<std::unique_ptr<KvPlottable>> plottables_;
};
