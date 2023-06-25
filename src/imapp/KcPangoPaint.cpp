#include "KcPangoPaint.h"
#include "pango/pangocairo.h"
#include "cairo.h"
#include "plot/backend/cairo/KcCairoSurfaceImage.h"
#include "glad.h"

#include "stb_image_write.h"

//#define STB_RECT_PACK_IMPLEMENTATION
//#include "imstb_rectpack.h"

KcPangoPaint::KcPangoPaint()
{
    cairoSurf_ = std::make_unique<KcCairoSurfaceImage>();

    glGenTextures(1, &texId_);
    glBindTexture(GL_TEXTURE_2D, texId_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //expandSurf_();

    //auto w = cairo_image_surface_get_width((cairo_surface_t*)cairoSurf_);
    //auto h = cairo_image_surface_get_height((cairo_surface_t*)cairoSurf_);
    //auto rp = new stbrp_context;
    //auto nodes = new stbrp_node[1024];
    //rectpack_ = rp;
    //rpnodes_ = nodes;
    //nnodes_ = 1024;
   // stbrp_init_target(rp, w, h, nodes, nnodes_);
   // stbrp_setup_allow_out_of_mem((stbrp_context*)rectpack_, 1);
}


KcPangoPaint::~KcPangoPaint()
{
   // auto w = cairo_image_surface_get_width((cairo_surface_t*)cairoSurf_);
   // auto h = cairo_image_surface_get_height((cairo_surface_t*)cairoSurf_);
   // auto data = cairo_image_surface_get_data((cairo_surface_t*)cairoSurf_);
    //stbi_write_bmp("font.bmp", w, h, 4, data);

    if (pangoLayout_)
        g_object_unref((PangoLayout*)pangoLayout_);

    glDeleteTextures(1, &texId_);

    //delete (stbrp_context*)rectpack_;
    //delete[] (stbrp_node*)rpnodes_;
}


void KcPangoPaint::beginPaint()
{
    super_::beginPaint();

    if (cairoSurf_->create(viewport())) { // 默认当前vp为canvas大小

        // 若创建了新的surface，须同步更新pango-layout
        if (pangoLayout_)
            g_object_unref((PangoLayout*)pangoLayout_);
        pangoLayout_ = pango_cairo_create_layout((cairo_t*)cairoSurf_->cr());
    }
    else {
        pango_cairo_update_layout((cairo_t*)cairoSurf_->cr(), (PangoLayout*)pangoLayout_);
    }

    cairoSurf_->clear({ 0, 0, 0, 0 });
}


void KcPangoPaint::drawRenderList_()
{
    super_::drawRenderList_();

    cairo_surface_flush((cairo_surface_t*)cairoSurf_->surface());
     
    auto surf = dynamic_cast<KcCairoSurfaceImage*>(cairoSurf_.get());
    auto data = surf->data();

    auto rc = cairoSurf_->canvas();
    glRasterRect_(rc);

    //glRasterPos2f(rc.lower().x(), rc.lower().y());
    //glRasterPos2f(0.5, 0.5);
    glDrawPixels(cairoSurf_->width(), cairoSurf_->height(), GL_RGBA, GL_UNSIGNED_BYTE, data);

/*    stbi_write_bmp("font.bmp", cairoSurf_->width(), cairoSurf_->height(), 4, data);

    // 将cairo表面绘制到opengl
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, texId_);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, cairoSurf_->width(), cairoSurf_->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-1, -1);
    glTexCoord2f(0, 1); glVertex2f(-1, 1);
    glTexCoord2f(1, 1); glVertex2f(1, 1);
    glTexCoord2f(1, 0); glVertex2f(1, -1);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();*/
}


void KcPangoPaint::expandSurf_()
{
    /*
    // 创建用于绘制文字的surface，默认大小256*256。尺寸不足时按2倍数扩充
    int w = 64, h = 64;  
    if (cairoSurf_) {
        w = cairo_image_surface_get_width((cairo_surface_t*)cairoSurf_);
        h = cairo_image_surface_get_height((cairo_surface_t*)cairoSurf_);
    }

    auto newSurf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w * 2, h * 2);
    auto newCxt = cairo_create((cairo_surface_t*)newSurf);
    cairo_set_source_rgb(newCxt, 0, 0, 0);
    cairo_paint(newCxt); // 黑底

    auto newLay = pango_cairo_create_layout(newCxt);
    
    if (cairoSurf_) {
        cairo_set_source_surface(newCxt, (cairo_surface_t*)cairoSurf_, 0, 0);
        cairo_paint(newCxt);

        g_object_unref((PangoLayout*)pangoLayout_);
        cairo_destroy((cairo_t*)cairoCxt_);
        cairo_surface_destroy((cairo_surface_t*)cairoSurf_);
    }

    pangoLayout_ = newLay;
    cairoCxt_ = newCxt;
    cairoSurf_ = newSurf;
    */
  //  if (rectpack_) {
  //      auto rp = (stbrp_context*)rectpack_;
  //      rp->width = w * 2;
   //     rp->height = h * 2; // TODO: reinit
   // }
}


void KcPangoPaint::expandNodes_()
{

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
    pango_cairo_show_layout((cairo_t*)cairoSurf_->cr(), (PangoLayout*)pangoLayout_);
}


void KcPangoPaint::setFont_() const
{
    assert(pangoLayout_);
    auto desc = pango_font_description_from_string("Sans Bold 13");
    pango_layout_set_font_description((PangoLayout*)pangoLayout_, desc);
    pango_font_description_free(desc);
}
