#pragma once
#include "KvPlot.h"
#include "KcCoord2d.h"
#include "KvRenderable.h"
#include "KtMargins.h"


// ��άplot�ĳ�����

class KvPlot2d : public KvPlot
{
public:
	using float_t = typename KvRenderable::float_t;
	using axis_ptr = std::shared_ptr<KcAxis>;
	using point2 = point2d;

	KvPlot2d(std::shared_ptr<KvPaint> paint);

	void update() override;

	void fitData() override;

	KcCoord2d& coord() { return *coord_.get(); }

	const KtMargins<float_t>& margins() const { return margins_; }
	KtMargins<float_t>& margins() { return margins_; }

protected:
	std::unique_ptr<KcCoord2d> coord_; // ���ô���������
	KtMargins<float_t> margins_{ 15, 15, 15, 15 };
};
