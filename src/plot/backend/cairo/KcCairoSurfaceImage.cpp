#include "KcCairoSurfaceImage.h"
#include "cairo.h"


void* KcCairoSurfaceImage::createSurface_() const
{
	return cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width(), height());
}
