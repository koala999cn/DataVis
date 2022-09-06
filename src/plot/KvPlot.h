#pragma once
#include <memory>
#include <vector>
#include <QColor>
#include "KvPlottable.h"
#include "KvCoordSystem.h"

class KvPlot
{
public:

	KvPlot(KvCoordSystem* coord) : coord_(coord) {
		bkgnd_ = Qt::white;
	}

	virtual void show(bool b) = 0;

	virtual bool visible() const = 0;

	virtual void* widget() const = 0;

	// ¸üÐÂ»æÍ¼
	virtual void update(bool immediately = true) = 0;

	QColor background() const { return bkgnd_; }

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
	QColor bkgnd_;
	std::unique_ptr<KvCoordSystem> coord_;
	std::vector<std::unique_ptr<KvPlottable>> plottables_;
};
