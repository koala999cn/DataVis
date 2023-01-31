#pragma once
#include "KcGraph.h"


//
// Andrews������һ�ֿ��ӻ���ά���ݵĻ���������
// �����˼����Ҫ���ú����任������Ԫ�����Զ�ά���ߵ���ʽչ�֡�
// �任�������£�
// f(t) = x1 / sqrt(2) + x2 * sin(t) + x3 * cos(t) + x4 * sin(2t) + x5 * cos(2t) + ��
//

class KcAndrewsCurves : public KcGraph
{
	using super_ = KcGraph;

public:

	using super_::super_;

	unsigned majorColorsNeeded() const override;

	aabb_t boundingBox() const override;

protected:

	void drawDiscreted_(KvPaint*, KvDiscreted*) const override;
};