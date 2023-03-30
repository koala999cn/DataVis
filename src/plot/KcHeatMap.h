#pragma once
#include "KvPlottable2d.h"
#include "KmLabeling.h"


// ��ͼ��ʵ��Ϊflat��Ⱦģʽ + text���� + ƽ�滯��surfaceͼ��ƫ��dx/2, -dy/2��

class KcHeatMap : public KvPlottable2d, public KmLabeling
{
	using super_ = KvPlottable2d;

public:

	KcHeatMap(const std::string_view& name);

	void setData(const_data_ptr d) override;

private:

	unsigned objectCount() const override;

	bool objectVisible_(unsigned objIdx) const override;

	bool objectReusable_(unsigned objIdx) const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const override;

	void* drawObject_(KvPaint*, unsigned objIdx) const final;

	void* drawGrid_(KvPaint*, unsigned) const;

	void* drawLabel_(KvPaint*) const;

	aabb_t calcBoundingBox_() const override;

	// ����xyƽ���ƫ����
	std::pair<float_t, float_t> xyshift_() const;
};
