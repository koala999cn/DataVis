#pragma once
#include <memory>
#include <vector>
#include "KvPlottable.h"
#include "KpContext.h"

class KvPaint; // ����ִ�о����plot����
class KvCoord;

// plot����ײ����ӿ�

class KvPlot
{
public:
	KvPlot(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord);

	virtual void setVisible(bool b) = 0;
	virtual bool visible() const = 0;

	virtual std::string title() const = 0;
	virtual std::string& title() = 0;

	virtual void fitData();

	virtual void update(); // ���»�ͼ

	const KpBrush& background() const { return bkgnd_; }
	KpBrush& background() { return bkgnd_; }

	bool autoFit() const { return autoFit_; }
	bool& autoFit() { return autoFit_; }

	KvCoord& coord() { return *coord_.get(); }

	unsigned plottableCount() const { return plottables_.size(); }

	KvPlottable* plottableAt(unsigned idx);

	// �ӹ�plot�Ŀ���Ȩ
	void addPlottable(KvPlottable* plt);

	// ��ǰ��idxλ�õ�plottable���󽫱��ͷ�
	void setPlottableAt(unsigned idx, KvPlottable* plt);

	void removePlottable(KvPlottable* plt);

	void removePlottableAt(unsigned idx);

	void removeAllPlottables();

protected:
	virtual void autoProject_() = 0;

protected:
	std::shared_ptr<KvPaint> paint_; // ���û�����������
	std::shared_ptr<KvCoord> coord_; // ���û�����������
	std::vector<std::unique_ptr<KvPlottable>> plottables_; // ���û�ͨ�����Ա��������

	KpBrush bkgnd_;

	bool autoFit_{ true }; // ��true����ÿ��update������������range�Զ���������ϵextents
};
