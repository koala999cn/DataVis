#pragma once
#include <memory>
#include <vector>
#include "KvPlottable.h"
#include "KpContext.h"
#include "KcLegend.h"
#include "KtAABB.h"
#include "layout/KgLayoutManager.h"


class KvPaint; // ����ִ�о����plot����
class KvCoord;


// plot����ײ����ӿ�

class KvPlot
{
	using rect_t = KtAABB<double, 2>;

public:
	KvPlot(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord);
	~KvPlot();

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

	bool showLegend() const { return showLegend_; }
	bool& showLegend() { return showLegend_; }

	KvCoord& coord() { return *coord_.get(); }

	KcLegend& legend() { return *legend_; }

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

	void updateLayout_(const rect_t& rc, void* cxt);

protected:
	std::shared_ptr<KvPaint> paint_; // ���û�����������
	std::shared_ptr<KvCoord> coord_; // ���û�����������
	std::unique_ptr<KcLegend> legend_; // �ڲ�����������
	std::vector<std::unique_ptr<KvPlottable>> plottables_; // ���û�ͨ�����Ա��������

	KpBrush bkgnd_;

	bool autoFit_{ true }; // ��true����ÿ��update������������range�Զ���������ϵextents
	bool showLegend_{ false };

	KgLayoutManager layMgr_;
};
