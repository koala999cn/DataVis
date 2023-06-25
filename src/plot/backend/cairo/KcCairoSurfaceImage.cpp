#include "KcCairoSurfaceImage.h"
#include "cairo.h"


void* KcCairoSurfaceImage::createSurface_() const
{
	return cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width(), height());
}


void* KcCairoSurfaceImage::data() const
{
	return cairo_image_surface_get_data((cairo_surface_t*)surf_);
}
