#pragma once
#include "KcColorMap.h"


// ����sampled2d���ݵ�3d����

class KcSurface : public KcColorMap
{
	using super_ = KcColorMap;

public:
	using super_::super_;


private:
	void drawDiscreted_(KvPaint*, KvDiscreted*) const final;
};
