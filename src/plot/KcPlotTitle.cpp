#include "KcPlotTitle.h"
#include "KvPaint.h"
#include "layout/KuLayoutUtil.h"


void KcPlotTitle::draw(KvPaint* paint) const
{
	super_::draw(paint); // ���Ʊ�����߿�

	auto sz = paint->textSize(name());
	auto rc = KuLayoutUtil::innerAlignedRect(innerRect(), sz, align(), true);
	
	paint->apply(font_);
	paint->drawText(rc.lower(), name());
}

