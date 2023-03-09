#pragma once
#include "KvPlottable1d.h"
#include <functional>


class KcLineFilled : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	unsigned majorColorsNeeded() const override;

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	unsigned objectCount() const final { return 2; } // fill��edge�ֱ���1����Ⱦ����

	const KpBrush& fillBrush() const { return fillCxt_; }
	KpBrush& fillBrush() { return fillCxt_; }

	const KpPen& linePen() const { return lineCxt_; }
	KpPen& linePen() { return lineCxt_; }

	bool showLine() const { return showLine_; }
	bool& showLine() { return showLine_; }

	// ���ͼ�Ļ���ģʽ
	enum KeFillMode
	{
		k_fill_overlay,
		k_fill_between,
		k_fill_delta
	};

	int fillMode() const { return fillMode_; }
	void setFillMode(KeFillMode mode);

	// ��׼��
	enum KeBaseMode
	{
		k_base_xline,
		k_base_yline,
		k_base_point
	};

	int baseMode() const { return baseMode_; }
	void setBaseMode(KeBaseMode mode);

	float_t baseLine() const { return baseLine_; }
	void setBaseLine(float_t base);

	const point3& basePoint() const { return basePoint_; }
	void setBasePoint(const point3& pt);

private:

	bool objectVisible_(unsigned objIdx) const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const final;

	bool objectReusable_(unsigned objIdx) const final;

	void* drawObject_(KvPaint*, unsigned objIdx) const final;

	void* fillOverlay_(KvPaint*) const;
	void* fillBetween_(KvPaint*, bool) const;
	void* fillDelta_(KvPaint*) const;
	void* fillRidge_(KvPaint*) const;

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;

	// ˫��fill
	void fillBetween_(KvPaint*, GETTER, GETTER, unsigned count, unsigned ch, void* vtx) const;

	// ����fill
	void fillBetween_(KvPaint*, const point3&, GETTER, unsigned count, unsigned ch, void* vtx) const;

	GETTER baseGetter_(unsigned ch, unsigned idx, GETTER g) const;
	point3 basePointAt_(unsigned ch, unsigned idx) const;

	// �������ڲ�overlayģʽ��line��Ŀ
	unsigned overlayCount_() const;

	// ����line��ch, idx����overlayģʽ�ĵڼ����߶�
	unsigned overlayIndex_(unsigned ch, unsigned idx) const;

	KuDataUtil::KpPointGetter1d lineOverlayed_(unsigned ch, unsigned idx, unsigned overlayIdx) const;

private:
	bool showLine_{ false };
	KpPen lineCxt_;
	KpBrush fillCxt_;
	int fillMode_{ 0 };
	int baseMode_{ 0 };
	float_t baseLine_{ 0 };
	point3 basePoint_{ 0, 0, 0 };
};