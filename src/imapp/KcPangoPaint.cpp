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

    if (cairoSurf_->create(viewport())) { // Ĭ�ϵ�ǰvpΪcanvas��С

        // ���������µ�surface����ͬ������pango-layout
        if (pangoLayout_)
            g_object_unref((PangoLayout*)pangoLayout_);
        pangoLayout_ = pango_cairo_create_layout((cairo_t*)cairoSurf_->cr());

        // TODO: pango_cairo_context_set_resolution(dpi);
    }

    cairoSurf_->clear({ 0, 0, 0, 0 });

    auto cxt = pango_layout_get_context((PangoLayout*)pangoLayout_);
    auto opt = cairo_font_options_create();
    cairo_font_options_set_antialias(opt, antialiasing() ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);
    pango_cairo_context_set_font_options(cxt, opt); // TODO: ���忹����޷���̬��������
    //pango_cairo_update_context((cairo_t*)cairoSurf_->cr(), cxt);
    //pango_layout_context_changed((PangoLayout*)pangoLayout_);
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

    // ����gl״̬�����ָ�
    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glClipPlane_(-1); // NB: ��plot3d����Ҫ��

    glViewport(rc.lower().x(), rc.lower().y(), rc.width(), rc.height()); // glRasterPos2f����������ڵ�ǰvp��������������vp
    glPixelZoom(1, -1); // ���µߵ�����
    glRasterPos2i(-1, 1); // �趨glDrawPixels��������(-1, -1)�����ǵ�zoom���˴��趨(-1, 1)
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
    auto sz = textSize(text); // textSize������pangoLayout_������
    auto pos = projectp(topLeft);
    cairo_move_to((cairo_t*)cairoSurf_->cr(), pos.x(), pos.y());
    cairoSurf_->setColor(clr_); // ����������ɫ

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
    return KvPaint::drawTexts(anchors, texts, align, spacing); // ����opengl���٣�ʹ��pango��Ⱦ
}


void KcPangoPaint::setFont_() const
{
    assert(pangoLayout_);

    auto desc = pango_font_description_from_string(family_.c_str());
    pango_font_description_set_size(desc, ftSize_ * PANGO_SCALE);
    pango_font_description_set_weight(desc, bold_ ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
    pango_font_description_set_style(desc, italic_ ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL);
    pango_layout_set_font_description((PangoLayout*)pangoLayout_, desc);
    pango_font_description_free(desc);

    // TODO: �˴����ܻ�ȡ���е�attrlist�������޷���������. WHY???
    PangoAttrList* attrlist = 0; // pango_layout_get_attributes((PangoLayout*)pangoLayout_);
    if (!attrlist) {
        attrlist = pango_attr_list_new();
        pango_layout_set_attributes((PangoLayout*)pangoLayout_, attrlist);
    }
    auto attr = pango_attr_underline_new(underline_ ? PANGO_UNDERLINE_SINGLE_LINE : PANGO_UNDERLINE_NONE);
    pango_attr_list_change(attrlist, attr);
    //pango_attribute_destroy(attr); NB: �����ͷţ�����crash
    pango_attr_list_unref(attrlist);
}
