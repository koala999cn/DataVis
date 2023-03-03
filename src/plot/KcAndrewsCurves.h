#pragma once
#include "KcGraph.h"


//
// Andrews曲线是一种可视化高维数据的基本方法。
// 其核心思想主要运用函数变换，将多元数据以二维曲线的形式展现。
// 变换函数如下：
// f(t) = x1 / sqrt(2) + x2 * sin(t) + x3 * cos(t) + x4 * sin(2t) + x5 * cos(2t) + …
//

class KcAndrewsCurves : public KcGraph
{
	using super_ = KcGraph;

public:

	using super_::super_;

	const_data_ptr odata() const { return curves_; }

private:

	// 生成andrews曲线
	virtual void outputImpl_();

private:
	data_ptr curves_; // 生成的andrews曲线
};
