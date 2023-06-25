#pragma once
#include "KvCairoSurface.h"


class KcCairoSurfaceImage : public KvCairoSurface
{
public:
	void* data() const;

protected:
	void* createSurface_() const override;
};