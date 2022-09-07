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

	// ��֪KvPlot��ͼ�����ǱȽ��ȶ�����̬���ݣ����Ǹ���Ƶ������̬���ݣ����Ա��Ż���ʾ
	virtual void setImmutable(bool b) = 0;

	// ���»�ͼ
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
