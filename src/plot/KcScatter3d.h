#pragma once
#include "KvScatter.h"
#include "KpContext.h"

// 3dɢ��ͼ

class KcScatter3d : public KvScatter
{
	using super_ = KvScatter;

public:

	using super_::super_;

	void draw(KvPaint*) const override;

};
