#include "KcCairoPaintSvg.h"
#include "cairo/cairo-svg.h" 


KcCairoPaintSvg::KcCairoPaintSvg(const std::string_view& path)
    : svgpath_(path)
{

}


KcCairoPaintSvg::KcCairoPaintSvg(const std::string_view& path, const rect_t& canvas)
    : KcCairoPaintSvg(path)
{
    setRect(canvas);
}


void* KcCairoPaintSvg::createSurface_() const
{
    return cairo_svg_surface_create(svgpath_.c_str(), canvas_.width(), canvas_.height());
}
