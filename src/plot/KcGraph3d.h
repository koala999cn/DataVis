#pragma once
#include "KvGraph.h"
#include "KpContext.h"

// 3d����ͼ

class KcGraph3d : public KvGraph
{
	using super_ = KvGraph;

public:

	using super_::super_;

	void draw(KvPaint*) const override;

};
