#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"


// 序列图的基类，主要处理串行数据，用于绘制折线图graph、散点图scatter等
// 兼容2d和3d模式

class KvPlottable1d : public KvPlottable
{
public:

	using KvPlottable::KvPlottable;

protected:

	unsigned objectCount() const override;

	// 绘制第objIdx个渲染对象，并返回可复用的对象id
	void* drawObject_(KvPaint*, unsigned objIdx) const override;

	// 每个批次的渲染对象数目，有的实现可能fill和edge分别有1个对象，有的可能text还有1个对象
	// 通常每个通道对用1个批次
	virtual unsigned objectsPerBatch_() const { return 1; }

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;

	// 为了兼容连续数据，此处增加unsigned参数，表示数据count
	// GETTER不作z替换（数据dim>1时），但完成z附加（数据dim=1时），确保返回的数据尺寸 >= 3
	// 继承类须根据forceDefaultZ对GETTER返回值作二次处理（自行完成z值替换，或其他操作）
	virtual void* drawObjectImpl_(KvPaint*, GETTER, unsigned count, unsigned objIdx) const = 0;

	// 将GETTER变量转换为KvPaint需要的函数型
	typename KvPaint::point_getter1 toPoint3Getter_(GETTER g, unsigned channel) const;

	// 将渲染对象id转换为通道号
	unsigned objIdx2ChsIdx_(unsigned objIdx) const;

private:

	// 一维数据(x, y)在x-y平面绘图（使用缺省z值）
	void* draw1d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

	// 二维数据(x, y, z)画瀑布图, 以x为流动轴
	void* draw2d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

	// 二维数据(x, y, z)画3d图
	void* draw3d_(KvPaint*, unsigned objIdx, const KvDiscreted*) const;

};
