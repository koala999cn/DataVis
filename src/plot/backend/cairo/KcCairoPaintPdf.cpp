#include "KcCairoPaintPdf.h"
#include "cairo/cairo-pdf.h" 


KcCairoPaintPdf::KcCairoPaintPdf(const std::string_view& path)
    : pdfpath_(path)
{

}


KcCairoPaintPdf::KcCairoPaintPdf(const std::string_view& path, const rect_t& canvas)
    : KcCairoPaintPdf(path)
{
    setRect(canvas);
}


void* KcCairoPaintPdf::createSurface_() const
{
    return cairo_pdf_surface_create(pdfpath_.c_str(), canvas_.width(), canvas_.height());
}
