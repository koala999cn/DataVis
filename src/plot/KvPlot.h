#pragma once
#include <memory>
#include <vector>
#include "KvPlottable.h"
#include "KpContext.h"
#include "KtAABB.h"
#include "layout/KcLayoutGrid.h"
#include "KvDecorator.h"


class KvPaint; // ����ִ�о����plot����
class KvCoord;
class KcLegend;
class KcColorBar;

// plot����ײ����ӿ�

class KvPlot : public KvDecorator, public KcLayoutGrid
{
	using rect_t = KtAABB<double, 2>;
	using margins_t = KtMargins<float>;

public:
	KvPlot(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord, char dim);
	~KvPlot();

	aabb_t boundingBox() const override {
		auto& rc = innerRect();
		return { point3d(rc.lower().x(), rc.lower().y(), 0),
			point3d(rc.upper().y(), rc.upper().y(), 0) };
	}

	// @paint: ȱʡnullptr����ʾʹ������KvPaint�������ô����paint���л��ƣ�������ʵ�ֵ�������
	virtual void update(KvPaint* paint = nullptr); // ���»�ͼ

	void fitData();

	bool showLayoutRect() const { return showLayoutRect_; }
	bool& showLayoutRect() { return showLayoutRect_; }

	KvPaint& paint() { return *paint_.get(); }
	const KvPaint& paint() const { return *paint_.get(); }
	KvCoord& coord() { return *coord_.get(); }

	// �����ⲿ��ȡ�͸���legend��colorbars������
	KcLegend* legend() const { return legend_.get(); }
	unsigned colorbarCount() const { return colorbars_.size(); }
	KcColorBar* colorbarAt(unsigned idx) const { return colorbars_[idx].get(); }

	unsigned plottableCount() const { return plottables_.size(); }

	KvPlottable* plottableAt(unsigned idx);

	// �ӹ�plot�Ŀ���Ȩ
	void addPlottable(KvPlottable* plt);

	// ��ǰ��idxλ�õ�plottable���󽫱��ͷ�
	void setPlottableAt(unsigned idx, KvPlottable* plt);

	void removePlottable(KvPlottable* plt);

	void removePlottableAt(unsigned idx);

	void removeAllPlottables();

	char dim() const { return dim_; }

private:

	using KvDecorator::draw; // ��ֹ�ⲿ���ʸó�Ա����

	virtual void autoProject_() = 0;

	bool showLegend_() const;

	void updateLayout_(KvPaint*, const rect_t& rc);

	// ��������plottables��ɫ��ģʽ��������legend��colorbar
	void syncLegendAndColorbars_();

	void drawPlottables_(KvPaint*);

	// ������ͼ�ӿڵ�ƫ�ƺ����ţ���plot2d����Ҫ��
	// ����ѹ���local�任��������
	int fixPlotView_(KvPaint*);

	// ���Ƹ�����Ԫ�ص���߿�����debugʹ��
	void drawLayoutRect_(KvPaint*);

	// ��legend��colorbarsԪ�ش�layoutϵͳ���Ƴ�������ͬ�����߷�ֹ��layoutϵͳЧ����
	void unlayoutLegendAndColorbars_();

private:
	std::shared_ptr<KvPaint> paint_; // ���û�������ͨ�����캯������
	std::shared_ptr<KvCoord> coord_; // ���û�������ͨ�����캯������
	std::unique_ptr<KcLegend> legend_; // ����plottables_�Զ�����������
	std::vector<std::unique_ptr<KcColorBar>> colorbars_; // ����plottables_�Զ�����������֧�ֶ��ɫ��
	std::vector<std::unique_ptr<KvPlottable>> plottables_; // ���û�ͨ�����Ա��������

	char dim_{ 3 }; // ȡֵ2��3���������this��plot2d����plot3d

	bool showLayoutRect_{ false }; // for debug
};
