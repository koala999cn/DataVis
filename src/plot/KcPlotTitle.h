#pragma once
#include "KvDecoratorAligned.h"


class KcPlotTitle : public KvDecoratorAligned
{
	using super_ = KvDecoratorAligned;

public:
	virtual void draw(KvPaint*) const;

private:
	KpFont font_; // titleµÄ×ÖÌå
};
