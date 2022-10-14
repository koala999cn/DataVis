#pragma once
#include "KvPlot.h"
#include "KcCoord2d.h"
#include "KvRenderable.h"


// ��άplot�ĳ�����

class KvPlot2d : public KvPlot
{
public:
	using float_type = typename KvRenderable::float_type;
	using axis_ptr = std::shared_ptr<KcAxis>;
	using point2 = point2d;

	KvPlot2d(std::shared_ptr<KvPaint> paint);

	void update() override;

	void fitData() override;

	KcCoord2d& coord() { return *coord_.get(); }

protected:
	std::unique_ptr<KcCoord2d> coord_; // ���ô���������
};
