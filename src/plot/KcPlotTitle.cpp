#include "KcPlotTitle.h"
#include "KvPaint.h"
#include "layout/KuLayoutUtil.h"


KcPlotTitle::KcPlotTitle(const std::string_view& title)
	: super_(title)
{
	showBorder() = false; showBkgnd() = false;
	setMargins(5, 5, 5, 5);
	font_.size = 16;
	align_ = KeAlignment::k_hcenter | KeAlignment::k_vcenter;
}


void KcPlotTitle::draw(KvPaint* paint) const
{
	super_::draw(paint); // »æÖÆ±³¾°Óë±ß¿ò
	paint->apply(font_);
	paint->setColor(color_);
	auto& pt = innerRect().lower();
	paint->drawText({ pt.x(), pt.y() }, name());
}


KcPlotTitle::size_t KcPlotTitle::calcSize_(void* cxt) const
{
	auto paint = (KvPaint*)cxt;
	paint->apply(font_);
	return paint->textSize(name());
}


