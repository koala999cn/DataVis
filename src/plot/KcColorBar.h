#pragma once
#include "KvDecoratorAligned.h"
#include "KpContext.h"
#include "KcAxis.h"

class KvPlottable;

class KcColorBar : public KvDecoratorAligned
{
	using super_ = KvDecoratorAligned;

public:

	KcColorBar(KvPlottable* plt);

	void resetPlottable(KvPlottable* plt);

	void draw(KvPaint*) const override;

	bool showBarBorder() const { return showBarBorder_; }
	bool& showBarBorder() { return showBarBorder_; }

	const KpPen& barPen() const { return barPen_; }
	KpPen& barPen() { return barPen_; }

	float barWidth() const { return barWidth_; }
	float& barWidth() { return barWidth_; }

	float barLength() const { return barLength_; }
	float& barLength() { return barLength_; }

	const KcAxis& axis() const { return *axis_; }
	KcAxis& axis() { return *axis_; }

private:

	size_t calcSize_(void*) const final;

private:

	bool showBarBorder_{ true };
	KpPen barPen_;

	float barWidth_{ 24 }; // ����ֵ
	float barLength_{ 0 }; // ����ֵ. 0��ʾ��չ��coord-plane����
	int ticks_{ 0 }; // 0��ʾautotick

	KvPlottable* plt_{ nullptr };
	
	std::unique_ptr<KcAxis> axis_; // ���ڻ���tick��label
};
