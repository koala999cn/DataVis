#pragma once
#include "KvCairoSurface.h"


class KcCairoSurfaceImage : public KvCairoSurface
{
protected:
	void* createSurface_() const override;
};