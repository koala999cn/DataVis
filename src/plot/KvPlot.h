#pragma once
#include <memory>
#include <vector>
#include "KvPlottable.h"
#include "KpContext.h"
#include "KtAABB.h"
#include "KtMargins.h"


class KvPaint; // ����ִ�о����plot����
class KvCoord;
class KcLegend;
class KcColorBar;
class KcLayoutGrid;

// plot����ײ����ӿ�

class KvPlot
{
	using rect_t = KtAABB<double, 2>;
	using margins_t = KtMargins<float>;

public:
	KvPlot(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord, char dim);
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

	bool showColorBar() const { return showColorBar_; }
	bool& showColorBar() { return showColorBar_; }

	bool showLayoutRect() const { return showLayoutRect_; }
	bool& showLayoutRect() { return showLayoutRect_; }

	KvPaint& paint() { return *paint_.get(); }
	KvCoord& coord() { return *coord_.get(); }

	KcLegend* legend() const { return legend_; }
	KcColorBar* colorBar() const { return colorBar_; }

	unsigned plottableCount() const { return plottables_.size(); }

	KvPlottable* plottableAt(unsigned idx);

	// �ӹ�plot�Ŀ���Ȩ
	void addPlottable(KvPlottable* plt);

	// ��ǰ��idxλ�õ�plottable���󽫱��ͷ�
	void setPlottableAt(unsigned idx, KvPlottable* plt);

	void removePlottable(KvPlottable* plt);

	void removePlottableAt(unsigned idx);

	void removeAllPlottables();

	rect_t canvasRect() const;

	margins_t margins() const;
	void setMargins(const margins_t& m);
	void setMargins(float l, float t, float r, float b);

	char dim() const { return dim_; }

private:
	virtual void autoProject_() = 0;

	void updateLayout_(const rect_t& rc);

	bool realShowLegend_() const;
	bool realShowColorBar_() const;

	void syncLegendAndColorBar_(KvPlottable* removed, KvPlottable* added);

	void drawPlottables_();

	// ������ͼ�ӿڵ�ƫ�ƺ����ţ���plot2d����Ҫ��
	// ����ѹ���local�任��������
	int fixPlotView_();

	// ���Ƹ�����Ԫ�ص���߿�����debugʹ��
	void drawLayoutRect_();

private:
	std::shared_ptr<KvPaint> paint_; // ���û�����������
	std::shared_ptr<KvCoord> coord_; // ���û�����������
	KcLegend* legend_; // �ڲ�����������
	KcColorBar* colorBar_; // �ڲ�����������
	std::vector<std::unique_ptr<KvPlottable>> plottables_; // ���û�ͨ�����Ա��������

	KpBrush bkgnd_;

	std::unique_ptr<KcLayoutGrid> layout_;

	char dim_{ 3 }; // ȡֵ2��3���������this��plot2d����plot3d

	bool autoFit_{ true }; // ��true����ÿ��update������������range�Զ���������ϵextents
	bool showLegend_{ false };
	bool showColorBar_{ true };
	bool showLayoutRect_{ false }; // for debug
};
