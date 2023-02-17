#pragma once
#include "KvPlottable.h"


// ����ͼʵ��
// ��ɫ���ڻ������壬��ɫ���ڻ���������߿�

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

	/// ����������
	KpMarker outlierMarker_; // �쳣��
	KpPen borderPen_; // ������߿�
	KpPen medianPen_; // ��ֵ��
	KpPen whisPen_; // �����ߣ�the lines which reach from the upper quartile to the maximum, and from the lower quartile to the minimum.
	KpPen whisBarPen_; // ������ߣ�the lines parallel to the key axis at each end of the whisker backbone

	float boxWidth_{ 0.1f }; // ����Ŀ��ϵ���������dx��ʵ�ʿ�ȵ���dx * width_
	float whisBarWidth_{ 0.08 }; // ������ߵĳ��ȣ������dx��Ĭ��ΪboxWidth_��һ��. NB: dx==1
	float whisFactor_{ 1.5f }; // �����볤��ϵ����Ĭ��Ϊ1.5����IQR. ȡ0ʱ����������Ϊ�����Сֵ����ʱ��outliers

	// ͳ�ƽ��
	struct KpBoxStat_
	{
		float_t median; // ��λ��
		float_t q1, q3; // 25%��75%��λ��
		float_t lower, upper; // ���±�Ե
		std::vector<float_t> outliers; // �쳣ֵ���������±�Ե��ֵ��
	};

	mutable std::vector<KpBoxStat_> stats_; // size����data��ͨ����
};
