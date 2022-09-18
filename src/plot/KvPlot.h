#pragma once
#include <memory>
#include <vector>
#include "KtColor.h"

class KvPlottable;
class KcCoordSystem;

class KvPlot
{
public:

	KvPlot();

	virtual void show(bool b) = 0;

	virtual bool visible() const = 0;

	virtual void* widget() const = 0;

	virtual color4f background() const = 0;
	virtual void setBackground(const color4f& clr) = 0;

	// ���»�ͼ
	virtual void update();

	KcCoordSystem* coordSystem() {
		return coord_.get();
	}

	unsigned plottableCount() const {
		return plottables_.size();
	}

	KvPlottable* plottable(unsigned idx);

	// �ӹ�plot�Ŀ���Ȩ
	void addPlottable(KvPlottable* plot);

protected:
	virtual void updateImpl_() = 0;
	virtual void autoProject_() = 0;
	virtual void autoFit_();
	
protected:
	std::unique_ptr<KcCoordSystem> coord_;
	std::vector<std::unique_ptr<KvPlottable>> plottables_;
	bool ortho_; // ����ͶӰ vs. ͸��ͶӰ
	bool fitData_; // ��������range�Զ���������ϵextents
};
