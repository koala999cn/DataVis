#pragma once
#include <memory>
#include <vector>
#include "KvPlottable.h"
#include "KtColor.h"

class KvPaint; // ����ִ�о����plot����

class KvPlot
{
public:

	KvPlot(std::shared_ptr<KvPaint> paint);

	virtual void setVisible(bool b) = 0;
	virtual bool visible() const = 0;

	virtual color4f background() const = 0;
	virtual void setBackground(const color4f& clr) = 0;

	virtual void fitData() = 0;

	virtual void update(); // ���»�ͼ

	bool isAutoFit() const { return autoFit_; }
	void setAutoFit(bool b) { autoFit_ = b; }

	unsigned plottableCount() const {
		return plottables_.size();
	}

	KvPlottable* plottable(unsigned idx);

	// �ӹ�plot�Ŀ���Ȩ
	void addPlottable(KvPlottable* plot);

protected:
	std::shared_ptr<KvPaint> paint_; // ���û�����������
	std::vector<std::unique_ptr<KvPlottable>> plottables_; // ���û�ͨ�����Ա��������

	bool autoFit_{ true }; // ��true����ÿ��update������������range�Զ���������ϵextents
};
