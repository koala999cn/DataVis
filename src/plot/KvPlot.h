#pragma once
#include <memory>
#include <vector>
#include "KvPlottable.h"
#include "KpContext.h"

class KvPaint; // ����ִ�о����plot����

// plot����ײ����ӿ�

class KvPlot
{
public:
	KvPlot(std::shared_ptr<KvPaint> paint);

	virtual void setVisible(bool b) = 0;
	virtual bool visible() const = 0;

	virtual void fitData() = 0;

	virtual void update(); // ���»�ͼ

	const KpBrush& background() const { return bkgnd_; }
	KpBrush& background() { return bkgnd_; }

	bool autoFit() const { return autoFit_; }
	bool& autoFit() { return autoFit_; }

	unsigned plottableCount() const { return plottables_.size(); }

	KvPlottable* plottable(unsigned idx);

	// �ӹ�plot�Ŀ���Ȩ
	void addPlottable(KvPlottable* plot);

	void removePlottable(KvPlottable* plot);

	void removePlottable(unsigned idx);

	void removeAllPlottables();

protected:
	std::shared_ptr<KvPaint> paint_; // ���û�����������
	std::vector<std::unique_ptr<KvPlottable>> plottables_; // ���û�ͨ�����Ա��������

	KpBrush bkgnd_;

	bool autoFit_{ true }; // ��true����ÿ��update������������range�Զ���������ϵextents
};
