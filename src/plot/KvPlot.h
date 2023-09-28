#pragma once
#include <memory>
#include <vector>
#include "KvPlottable.h"
#include "KpContext.h"
#include "KtAABB.h"
#include "layout/KcLayoutGrid.h"
#include "KvDecorator.h"


class KvPaint; // 用来执行具体的plot绘制
class KvCoord;
class KcLegend;
class KcColorBar;

// plot的最底层抽象接口

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

	// @paint: 缺省nullptr，表示使用内置KvPaint；否则用传入的paint进行绘制，可用于实现导出功能
	virtual void update(KvPaint* paint = nullptr); // 更新绘图

	void fitData();

	bool showLayoutRect() const { return showLayoutRect_; }
	bool& showLayoutRect() { return showLayoutRect_; }

	KvPaint& paint() { return *paint_.get(); }
	const KvPaint& paint() const { return *paint_.get(); }
	KvCoord& coord() { return *coord_.get(); }

	// 用于外部获取和更改legend与colorbars的属性
	KcLegend* legend() const { return legend_.get(); }
	unsigned colorbarCount() const { return colorbars_.size(); }
	KcColorBar* colorbarAt(unsigned idx) const { return colorbars_[idx].get(); }

	unsigned plottableCount() const { return plottables_.size(); }

	KvPlottable* plottableAt(unsigned idx);

	// 接管plot的控制权
	void addPlottable(KvPlottable* plt);

	// 当前在idx位置的plottable对象将被释放
	void setPlottableAt(unsigned idx, KvPlottable* plt);

	void removePlottable(KvPlottable* plt);

	void removePlottableAt(unsigned idx);

	void removeAllPlottables();

	char dim() const { return dim_; }

private:

	using KvDecorator::draw; // 禁止外部访问该成员函数

	virtual void autoProject_() = 0;

	bool showLegend_() const;

	void updateLayout_(KvPaint*, const rect_t& rc);

	// 根据所含plottables的色彩模式重新配置legend和colorbar
	void syncLegendAndColorbars_();

	void drawPlottables_(KvPaint*);

	// 修正绘图视口的偏移和缩放（对plot2d很重要）
	// 返回压入的local变换矩阵数量
	int fixPlotView_(KvPaint*);

	// 绘制各布局元素的外边框，用于debug使用
	void drawLayoutRect_(KvPaint*);

	// 将legend和colorbars元素从layout系统中移除（重新同步或者防止被layout系统效果）
	void unlayoutLegendAndColorbars_();

private:
	std::shared_ptr<KvPaint> paint_; // 由用户创建并通过构造函数传入
	std::shared_ptr<KvCoord> coord_; // 由用户创建并通过构造函数传入
	std::unique_ptr<KcLegend> legend_; // 根据plottables_自动创建并管理
	std::vector<std::unique_ptr<KcColorBar>> colorbars_; // 根据plottables_自动创建并管理，支持多个色带
	std::vector<std::unique_ptr<KvPlottable>> plottables_; // 由用户通过类成员方法管理

	char dim_{ 3 }; // 取值2或3，用来标记this是plot2d还是plot3d

	bool showLayoutRect_{ false }; // for debug
};
