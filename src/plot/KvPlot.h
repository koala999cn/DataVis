#pragma once
#include <memory>
#include <vector>
#include "KvPlottable.h"
#include "KpContext.h"
#include "KcLegend.h"
#include "KtAABB.h"
#include "layout/KgLayoutManager.h"


class KvPaint; // 用来执行具体的plot绘制
class KvCoord;


// plot的最底层抽象接口

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

	virtual void update(); // 更新绘图

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

	// 接管plot的控制权
	void addPlottable(KvPlottable* plt);

	// 当前在idx位置的plottable对象将被释放
	void setPlottableAt(unsigned idx, KvPlottable* plt);

	void removePlottable(KvPlottable* plt);

	void removePlottableAt(unsigned idx);

	void removeAllPlottables();


protected:
	virtual void autoProject_() = 0;

	void updateLayout_(const rect_t& rc, void* cxt);

protected:
	std::shared_ptr<KvPaint> paint_; // 由用户创建并传入
	std::shared_ptr<KvCoord> coord_; // 由用户创建并传入
	std::unique_ptr<KcLegend> legend_; // 内部创建并管理
	std::vector<std::unique_ptr<KvPlottable>> plottables_; // 由用户通过类成员方法管理

	KpBrush bkgnd_;

	bool autoFit_{ true }; // 若true，则每次update都将根据数据range自动调整坐标系extents
	bool showLegend_{ false };

	KgLayoutManager layMgr_;
};
