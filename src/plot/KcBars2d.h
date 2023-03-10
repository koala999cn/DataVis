#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"
#include <functional>


// 2ά��״ͼ.
// ֧�ֶѵ���stacked�������飨grouped�����Լ��ѵ�+����ģʽ

class KcBars2d : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	KcBars2d(const std::string_view& name);

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	unsigned objectCount() const override;

	bool showFill() const { return filled_; }
	bool& showFill() { return filled_; }

	bool showBorder() const { return edged_; }
	bool& showBorder() { return edged_; }

	// bar�Ŀ�����ã�ȡֵ[0, 1]
	// ��ȡ1����bars֮���޿�϶����ȡ0.5����bar�Ŀ����bars֮��Ŀ�϶��ͬ��
	float barWidthRatio() const { return barWidthRatio_; }
	void setBarWidthRatio(float w);

	float baseLine() const { return baseLine_; }
	void setBaseLine(float base);

	const KpBrush& fillBrush() const { return fill_; }
	KpBrush& fillBrush() { return fill_; }

	const KpPen& borderPen() const { return border_; }
	KpPen& borderPen() { return border_; }

	float stackPadding() const { return stackPadding_; }
	void setStackPadding(float padding);


protected:

	void setObjectState_(KvPaint*, unsigned objIdx) const override;

	bool objectVisible_(unsigned objIdx) const override;

	void* drawObject_(KvPaint*, unsigned objIdx) const override;

	aabb_t calcBoundingBox_() const override;

	// ���㵥��bar�Ŀ�ȣ��������꣩
	float_t barWidth_(unsigned dim) const;

	bool realFilled_() const;

	bool realEdged_() const;

private:

	// ���ػ��Ƶ���bar��Ҫ��<������, ������>
	virtual std::pair<unsigned, unsigned> vtxSizePerBar_() const;

	// ���Ƶ���bar
	// @pos: ��ǰbar������ֵ���ߴ����dim+1
	// @ch: ��ǰͨ�����
	// @bottom: ����paddingStacked_�����ĵײ�ֵ
	// @vtx: ��д��Ķ������ݣ�����Ϊ{ float3, float4 }���ߴ����vtxSizePerBar_().first
	// @idx: ��д����������ݣ�����Ϊunsigned���ߴ����vtxSizePerBar_().second
	// @idxBase: ��������ʼֵ
	virtual void drawOneBar_(float_t* pos, unsigned ch, float_t bottom, void* vtx, void* idx, unsigned idxBase) const;

private:
	mutable KpBrush fill_;
	float barWidthRatio_{ 0.75f }; // barWidth = barWidthRatio_ * dx
	float baseLine_{ 0 }; // bar�ĵ��ߣ���bar�ĸ߶� = baseLine_ + y
	bool edged_{ true }, filled_{ true };
	KpPen border_;

	float stackPadding_{ 0.f }; // ÿ���ѵ�֮�ڵĸ�bar��ֱ���������ֵ��
};
