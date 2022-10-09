#pragma once
#include <memory>
#include <vector>
#include "KvPlottable.h"
#include "KtColor.h"

class KvPaint; // 用来执行具体的plot绘制

class KvPlot
{
public:

	KvPlot(std::shared_ptr<KvPaint> paint);

	virtual void setVisible(bool b) = 0;
	virtual bool visible() const = 0;

	virtual color4f background() const = 0;
	virtual void setBackground(const color4f& clr) = 0;

	virtual void fitData() = 0;

	virtual void update(); // 更新绘图

	bool isAutoFit() const { return autoFit_; }
	void setAutoFit(bool b) { autoFit_ = b; }

	unsigned plottableCount() const {
		return plottables_.size();
	}

	KvPlottable* plottable(unsigned idx);

	// 接管plot的控制权
	void addPlottable(KvPlottable* plot);

protected:
	std::shared_ptr<KvPaint> paint_; // 由用户创建并传入
	std::vector<std::unique_ptr<KvPlottable>> plottables_; // 由用户通过类成员方法管理

	bool autoFit_{ true }; // 若true，则每次update都将根据数据range自动调整坐标系extents
};
