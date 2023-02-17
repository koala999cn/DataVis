#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"

class KvSampled;

// imageͼ�Ļ��࣬��Ҫ����grid��sampled2d�����ݣ����ڻ�����ͼheatmap������ͼsurface��
// ����2d��3dģʽ

class KvPlottable2d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	bool showFill() const { return filled_; }
	bool& showFill() { return filled_; }

	bool showBorder() const { return edged_; }
	bool& showBorder() { return edged_; }

	const KpPen& borderPen() const { return borderPen_; }
	KpPen& borderPen() { return borderPen_; }

protected:

	unsigned renderObjectCount_() const override;

	void setRenderState_(KvPaint*, unsigned objIdx) const override;

	bool showFill_() const override;

	bool showEdge_() const override;

	void* drawObject_(KvPaint*, unsigned objIdx, const KvDiscreted* disc) const override;

	using GETTER = std::function<std::vector<float_t>(unsigned ix, unsigned iy)>;

	virtual void* drawImpl_(KvPaint*, GETTER, unsigned nx, unsigned ny, unsigned ch) const;

private:
	bool filled_{ true };
	bool edged_{ false };
	KpPen borderPen_;
};
