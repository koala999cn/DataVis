#include "KcPangoPaint.h"
#include "pango/pangocairo.h"
#include "cairo.h"
#include "plot/backend/cairo/KcCairoSurfaceImage.h"
#include "glad.h"


KcPangoPaint::KcPangoPaint()
{
    cairoSurf_ = std::make_unique<KcCairoSurfaceImage>();
}


KcPangoPaint::~KcPangoPaint()
{
    if (pangoLayout_)
        g_object_unref((PangoLayout*)pangoLayout_);
}


void KcPangoPaint::pushClipRect(const rect_t& cr, bool reset)
{
    super_::pushClipRect(cr, reset);
    cairoSurf_->setClipRect(cr, reset);
}


void KcPangoPaint::popClipRect()
{
    super_::popClipRect();
    cairoSurf_->setClipRect(clipRect(), true);
}


void KcPangoPaint::beginPaint()
{
    super_::beginPaint();

    if (cairoSurf_->create(viewport())) { // 默认当前vp为canvas大小

        // 若创建了新的surface，须同步更新pango-layout
        if (pangoLayout_)
            g_object_unref((PangoLayout*)pangoLayout_);
        pangoLayout_ = pango_cairo_create_layout((cairo_t*)cairoSurf_->cr());

        // TODO: pango_cairo_context_set_resolution(dpi);
    }

    cairoSurf_->clear({ 0, 0, 0, 0 });

    auto cxt = pango_layout_get_context((PangoLayout*)pangoLayout_);
    auto opt = cairo_font_options_create();
    cairo_font_options_set_antialias(opt, antialiasing() ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);
    pango_cairo_context_set_font_options(cxt, opt); // TODO: 字体抗锯齿无法动态更改配置
    pango_cairo_update_context((cairo_t*)cairoSurf_->cr(), cxt);
    pango_layout_context_changed((PangoLayout*)pangoLayout_);
    cairo_font_options_destroy(opt);

    //cairo_set_antialias((cairo_t*)cairoSurf_->cr(), antialiasing() ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);
}


void KcPangoPaint::drawRenderList_()
{
    super_::drawRenderList_();

    cairo_surface_flush((cairo_surface_t*)cairoSurf_->surface());
     
    auto surf = dynamic_cast<KcCairoSurfaceImage*>(cairoSurf_.get());
    auto data = surf->data();

    auto rc = cairoSurf_->canvas();
    glRasterRect_(rc);

    // 配置gl状态，不恢复
    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glClipPlane_(-1); // NB: 对plot3d很重要！

    glViewport(rc.lower().x(), rc.lower().y(), rc.width(), rc.height()); // glRasterPos2f的坐标相对于当前vp，所以须先重置vp
    glPixelZoom(1, -1); // 上下颠倒绘制
    glRasterPos2i(-1, 1); // 设定glDrawPixels绘制坐标(-1, -1)，考虑到zoom，此处设定(-1, 1)
    glDrawPixels(rc.width(), rc.height(), GL_BGRA, GL_UNSIGNED_BYTE, data);
    assert(glGetError() == GL_NO_ERROR);
}


KcPangoPaint::point2 KcPangoPaint::textSize(const std::string_view& text) const
{
    setFont_();
	pango_layout_set_text((PangoLayout*)pangoLayout_, text.data(), text.length());
    int w, h;
    pango_layout_get_size((PangoLayout*)pangoLayout_, &w, &h);
    return { (double)w / PANGO_SCALE, (double)h / PANGO_SCALE };
}


void KcPangoPaint::drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const std::string_view& text)
{
    auto sz = textSize(text); // textSize将配置pangoLayout_上下文
    auto pos = projectp(topLeft);
    cairo_move_to((cairo_t*)cairoSurf_->cr(), pos.x(), pos.y());
    cairoSurf_->setColor(clr_); // 设置字体颜色

    auto h = projectv(hDir);
    auto cr = (cairo_t*)cairoSurf_->cr();
    cairo_save(cr);
    cairo_rotate(cr, std::atan2(h.y(), h.x()));
    pango_cairo_update_layout(cr, (PangoLayout*)pangoLayout_);
    pango_cairo_show_layout(cr, (PangoLayout*)pangoLayout_);
    cairo_restore(cr);
}


void* KcPangoPaint::drawTexts(const std::vector<point3>& anchors, const std::vector<std::string>& texts, int align, const point2f& spacing)
{
    return KvPaint::drawTexts(anchors, texts, align, spacing); // 避免opengl加速，使用pango渲染
}


void KcPangoPaint::setFont_() const
{
    assert(pangoLayout_);
    std::string face(family_);
    face += " "; face += std::to_string(ftSize_); //face += "pt";
    if (bold_)
        face += " Bold";
    if (italic_)
        face += " Oblique";

    auto desc = pango_font_description_from_string(face.c_str());
    pango_layout_set_font_description((PangoLayout*)pangoLayout_, desc);
    pango_font_description_free(desc);
}
