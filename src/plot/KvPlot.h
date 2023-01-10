#pragma once
#include <memory>
#include <vector>
#include "KvPlottable.h"
#include "KpContext.h"
#include "KtAABB.h"
#include "KtMargins.h"


class KvPaint; // 用来执行具体的plot绘制
class KvCoord;
class KcLegend;
class KcColorBar;
class KcLayoutGrid;

// plot的最底层抽象接口

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

	virtual void update(); // 更新绘图

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

	// 接管plot的控制权
	void addPlottable(KvPlottable* plt);

	// 当前在idx位置的plottable对象将被释放
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

	// 修正绘图视口的偏移和缩放（对plot2d很重要）
	// 返回压入的local变换矩阵数量
	int fixPlotView_();

	// 绘制各布局元素的外边框，用于debug使用
	void drawLayoutRect_();

private:
	std::shared_ptr<KvPaint> paint_; // 由用户创建并传入
	std::shared_ptr<KvCoord> coord_; // 由用户创建并传入
	KcLegend* legend_; // 内部创建并管理
	KcColorBar* colorBar_; // 内部创建并管理
	std::vector<std::unique_ptr<KvPlottable>> plottables_; // 由用户通过类成员方法管理

	KpBrush bkgnd_;

	std::unique_ptr<KcLayoutGrid> layout_;

	char dim_{ 3 }; // 取值2或3，用来标记this是plot2d还是plot3d

	bool autoFit_{ true }; // 若true，则每次update都将根据数据range自动调整坐标系extents
	bool showLegend_{ false };
	bool showColorBar_{ true };
	bool showLayoutRect_{ false }; // for debug
};
