#pragma once
#include "KvPlottable.h"


// 箱线图实现
// 主色用于绘制箱体，辅色用于绘制箱体外边框

class KcBoxPlot : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	const color4f& minorColor() const override;
	void setMinorColor_(const color4f& minor) override;

	const KpMarker& outlierMarker() const { return outlierMarker_; }
	KpMarker& outlierMarker() { return outlierMarker_; }

	const KpPen& borderPen() const { return borderPen_; }
	KpPen& borderPen() { return borderPen_; }

	const KpPen& medianPen() const { return medianPen_; }
	KpPen& medianPen() { return medianPen_; }

	const KpPen& whisPen() const { return whisPen_; }
	KpPen& whisPen() { return whisPen_; }

	const KpPen& whisBarPen() const { return whisBarPen_; }
	KpPen& whisBarPen() { return whisBarPen_; }

	float boxWidth() const { return boxWidth_; }
	float& boxWidth() { return boxWidth_; }

	float whisBarWidth() const { return whisBarWidth_; }
	float& whisBarWidth() { return whisBarWidth_; }

	float whisLengthFactor() const { return whisFactor_; }
	float& whisLengthFactor() { return whisFactor_; }

private:

	unsigned renderObjectCount_() const override;

	void setRenderState_(KvPaint*, unsigned objIdx) const override;

	bool showFill_() const override;

	bool showEdge_() const override;

	void* drawObject_(KvPaint*, unsigned objIdx, const KvDiscreted* disc) const override;

	aabb_t calcBoundingBox_() const override;

	void calcStats_(const KvDiscreted*) const;

private:

	/// 绘制上下文
	KpMarker outlierMarker_; // 异常点
	KpPen borderPen_; // 箱体外边框
	KpPen medianPen_; // 中值线
	KpPen whisPen_; // 箱须线：the lines which reach from the upper quartile to the maximum, and from the lower quartile to the minimum.
	KpPen whisBarPen_; // 箱须端线：the lines parallel to the key axis at each end of the whisker backbone

	float boxWidth_{ 0.1f }; // 箱体的宽度系数，相对于dx，实际宽度等于dx * width_
	float whisBarWidth_{ 0.08 }; // 箱须端线的长度，相对于dx，默认为boxWidth_的一半. NB: dx==1
	float whisFactor_{ 1.5f }; // 上下须长度系数，默认为1.5倍的IQR. 取0时，置上下须为最大最小值，此时无outliers

	// 统计结果
	struct KpBoxStat_
	{
		float_t median; // 中位数
		float_t q1, q3; // 25%和75%分位数
		float_t lower, upper; // 上下边缘
		std::vector<float_t> outliers; // 异常值（超出上下边缘的值）
	};

	mutable std::vector<KpBoxStat_> stats_; // size等于data的通道数
};
