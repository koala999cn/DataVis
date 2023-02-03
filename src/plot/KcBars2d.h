#pragma once
#include "KvPlottable.h"
#include "KpContext.h"
#include <functional>


// 2ά��״ͼ.
// ֧�ֶѵ���stacked�������飨grouped�����Լ��ѵ�+����ģʽ

class KcBars2d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	KcBars2d(const std::string_view& name);

	aabb_t boundingBox() const override;

	unsigned majorColorsNeeded() const override;

	const color4f& minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	bool showFill() const { return showFill_; }
	bool& showFill() { return showFill_; }

	bool showBorder() const { return showBorder_; }
	bool& showBorder() { return showBorder_; }

	// bar�Ŀ�����ã�ȡֵ[0, 1]
	// ��ȡ1����bars֮���޿�϶����ȡ0.5����bar�Ŀ����bars֮��Ŀ�϶��ͬ��
	float barWidthRatio() const { return barWidthRatio_; }
	float& barWidthRatio() { return barWidthRatio_; }

	float baseLine() const { return baseLine_; }
	float& baseLine() { return baseLine_; }

	const KpBrush& fillBrush() const { return fill_; }
	KpBrush& fillBrush() { return fill_; }

	const KpPen& borderPen() const { return border_; }
	KpPen& borderPen() { return border_; }

	bool stackedFirst() const { return stackedFirst_; }
	bool& stackedFirst() { return stackedFirst_; }

	float paddingStacked() const { return paddingStacked_; }
	float& paddingStacked() { return paddingStacked_; }

	float paddingGrouped() const { return paddingGrouped_; }
	float& paddingGrouped() { return paddingGrouped_; }


private:

	void drawDiscreted_(KvPaint*, const KvDiscreted*) const override;

	// ���㵥��bar�Ŀ�ȣ��������꣩
	float_t barWidth_(unsigned dim = 0) const;

	struct KpEasyGetter
	{
		// ��ȡ��idx�صĵ�group���顢��stack�ѵ�����
		// NB: ��ʵ��Ϊ3��ά�ȵ�����
		using GETTER = std::function<std::vector<float_t>(unsigned idx, unsigned group, unsigned stack)>;

		unsigned clusters;
		unsigned groups;
		unsigned stacks;
		GETTER getter;
	};

	KpEasyGetter easyGetter_() const;

	unsigned xdim_() const;

	unsigned ydim_() const;

private:
	mutable KpBrush fill_;
	float barWidthRatio_{ 0.75f }; // barWidth = barWidthRatio_ * dx
	float baseLine_{ 0 }; // bar�ĵ��ߣ���bar�ĸ߶� = baseLine_ + y
	bool showBorder_{ true }, showFill_{ true };
	KpPen border_;
	bool stackedFirst_{ false }; // stacked���Ȼ���grouped����

	// ����2�����ֵ��Ϊ���ֵ��Ϊ��������������barWidth��
	float paddingStacked_{ 0.f }; // ÿ���ѵ�֮�ڵĸ�bar��ֱ���. 
	float paddingGrouped_{ 0.1f }; // ÿ������֮�ڵĸ�barˮƽ���. 
};
