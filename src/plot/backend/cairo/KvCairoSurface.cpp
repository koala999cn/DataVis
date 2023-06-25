#include "KvCairoSurface.h"
#include "cairo/cairo.h"

#define CAIRO_CXT reinterpret_cast<cairo_t*>(cxt_)
#define CAIRO_SURF reinterpret_cast<cairo_surface_t*>(surf_)


KvCairoSurface::~KvCairoSurface()
{
	destroy();
}


void KvCairoSurface::destroy()
{
	if (cxt_) {
		cairo_destroy(CAIRO_CXT);
		cxt_ = nullptr;
	}

	if (surf_) {
		cairo_surface_destroy(CAIRO_SURF);
		surf_ = nullptr;
	}

	canvas_.setNull();
}


void KvCairoSurface::setColor(const color4f& clr)
{
	cairo_set_source_rgba(CAIRO_CXT, clr.r(), clr.g(), clr.b(), clr.a());
}


bool KvCairoSurface::create(const rect_t& rc)
{
	assert(rc.volume() > 0);

	bool doTrans = (rc.lower() != canvas_.lower());
	bool doCreate = (canvas_.width() != rc.width() || canvas_.height() != rc.height());

	if (doCreate) {
		destroy();

		canvas_ = rc; // 须在调用createSurface_之前设置canvas_
		surf_ = createSurface_();
		cxt_ = cairo_create(CAIRO_SURF);
	}

	if (doTrans || doCreate)
		cairo_surface_set_device_offset(CAIRO_SURF, -rc.lower().x(), -rc.lower().y());

	return doCreate;
}


void KvCairoSurface::clear(const color4f& clr)
{
	cairo_save(CAIRO_CXT);
	setColor(clr);
	cairo_set_operator(CAIRO_CXT, CAIRO_OPERATOR_SOURCE);
	cairo_paint(CAIRO_CXT);
	cairo_restore(CAIRO_CXT);
}


void KvCairoSurface::setClipRect(const rect_t& rc)
{
	cairo_reset_clip(CAIRO_CXT);

	point2d pts[4];
	pts[0] = rc.lower(), pts[2] = rc.upper();
	pts[1] = { rc.lower().x(), rc.upper().y() };
	pts[3] = { rc.upper().x(), rc.lower().y() };

	cairo_move_to(CAIRO_CXT, pts[0].x(), pts[0].y());
	for (int i = 1; i < 4; i++)
		cairo_line_to(CAIRO_CXT, pts[i].x(), pts[i].y());
	cairo_close_path(CAIRO_CXT);

	cairo_clip(CAIRO_CXT);
}
