#include "KcCairoPaintPs.h"
#include "cairo-ps.h" 


KcCairoPaintPs::KcCairoPaintPs(const std::string_view& path)
    : pspath_(path)
{

}


KcCairoPaintPs::KcCairoPaintPs(const std::string_view& path, const rect_t& canvas)
    : KcCairoPaintPs(path)
{
    setRect(canvas);
}


void* KcCairoPaintPs::createSurface_() const
{
    return cairo_ps_surface_create(pspath_.c_str(), canvas_.width(), canvas_.height());
}
