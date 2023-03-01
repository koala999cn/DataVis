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

	unsigned majorColorsNeeded() const override;

private:

	unsigned channels_() const override;

	unsigned linesPerChannel_() const override;

	KuDataUtil::KpPointGetter1d lineAt_(unsigned ch, unsigned idx) const override;

	bool usingDefaultZ_() const override;

	aabb_t calcBoundingBox_() const override;

	// 生成andrews曲线
	void genCurves_();

private:
	std::shared_ptr<KvData> curves_; // 用来保存生成的andrews曲线，避免每个周期重新生成
};
