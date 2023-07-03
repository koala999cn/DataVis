#pragma once
#include "KvCairoSurface.h"
#include <string_view>


class KcCairoSurfaceImage : public KvCairoSurface
{
public:
	void* data() const;

	bool saveToFile(const std::string_view& path) const;

protected:
	void* createSurface_() const override;
};