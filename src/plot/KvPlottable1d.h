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

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;

	// 为了兼容连续数据，此处增加unsigned参数，表示数据count
	// GETTER不作z替换（数据dim>1时），但完成z附加（数据dim=1时），确保返回的数据尺寸 >= 3
	// 继承类须根据forceDefaultZ对GETTER返回值作二次处理（自行完成z值替换，或其他操作）
	virtual void drawImpl_(KvPaint*, GETTER, unsigned count, unsigned channel) const = 0;

	// 将GETTER变量转换为KvPaint需要的函数型
	typename KvPaint::point_getter1 toPointGetter_(GETTER g, unsigned channel) const;

private:

	// 一维数据(x, y)在x-y平面绘图（使用缺省z值）
	void draw1d_(KvPaint*, KvDiscreted*) const;

	// 二维数据(x, y, z)画瀑布图, 以x为流动轴
	void draw2d_(KvPaint*, KvDiscreted*) const;

	// 二维数据(x, y, z)画3d图
	void draw3d_(KvPaint*, KvDiscreted*) const;

};
