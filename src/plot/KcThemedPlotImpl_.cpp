#include "KcThemedPlotImpl_.h"
#include "KvPlot2d.h"
#include "KvCoord.h"
#include "KcCoordPlane.h"
#include "KcAxis.h"


KcThemedPlotImpl_::KcThemedPlotImpl_(KvPlot& plot)
    : plot_(plot)
{

}


KpBrush KcThemedPlotImpl_::fill(int level) const
{
	if (level == k_plot)
		return plot_.background();
	else if (level == k_axis) {
		auto& coord = plot_.coord();
		KpBrush bkgnd;
		coord.forPlane([&bkgnd](KcCoordPlane& plane) {
			bkgnd = plane.background();
			return false;
			});
		return bkgnd;
	}
	//	return qcp_->axisRect()->backgroundBrush();
	//else if (level == k_legend)
	//	return qcp_->legend->brush();
	//else
	//	assert(false);

	return KpBrush::null(); // make compiler happy
}


void KcThemedPlotImpl_::applyFill(int level, const KpBrush& b)
{
	if (level == k_plot)
		plot_.background() = b;
	else if (level == k_axis) {
		auto& coord = plot_.coord();
		coord.forPlane([&b](KcCoordPlane& plane) {
			plane.background() = b;
			return true;
			});
	}
}


KpPen KcThemedPlotImpl_::border(int level) const
{
	return KpPen::null();
}


void KcThemedPlotImpl_::applyBorder(int level, const KpPen&)
{

}


KtMargins<float> KcThemedPlotImpl_::margins(int level) const
{
	if (level == k_plot) {
		auto plot2d = dynamic_cast<KvPlot2d*>(&plot_);
		if (plot2d)
			return plot2d->margins();
	}

	return KtMargins<float>();
}


void KcThemedPlotImpl_::applyMargins(int level, const KtMargins<float>& margins)
{
	if (level == k_plot) {
		auto plot2d = dynamic_cast<KvPlot2d*>(&plot_);
		if (plot2d)
			plot2d->margins() = margins;
	}
	
}


bool KcThemedPlotImpl_::visible(int level) const
{
	return true;
}


void KcThemedPlotImpl_::applyVisible(int level, bool b)
{
	if (level & k_axis) {
		plot_.coord().forAxis([level, b](KcAxis& axis) {
			if (level & k_axis_tick_major)
				axis.showTick() = b;
			
			if (level & k_axis_tick_minor)
				axis.showSubtick() = b;

			if (level & k_axis_label)
				axis.showLabel() = b;

			if (level & k_axis_title)
				axis.showTitle() = b;

			return true;
			});
	}
	else if (level & k_grid) {
		plot_.coord().forPlane([level, b](KcCoordPlane& plane) {
			if (level & k_grid_major)
				plane.visible() = b;
			
			if (level & k_grid_minor)
				plane.minorVisible() = b;

			return true;
			});
	}
}


void KcThemedPlotImpl_::applyLine(int level, std::function<KpPen(const KpPen&)> op)
{

}


void KcThemedPlotImpl_::applyText(int level, std::function<KpFont(const KpFont&)> op)
{

}


void KcThemedPlotImpl_::applyTextColor(int level, std::function<color4f(const color4f&)> op)
{

}


void KcThemedPlotImpl_::setTickLength(int level, KeTickSide side, int len)
{

}


unsigned KcThemedPlotImpl_::plottableCount() const
{
    return plot_.plottableCount();
}


unsigned KcThemedPlotImpl_::majorColorsNeeded(unsigned idx) const
{
    return plot_.plottable(idx)->majorColorsNeeded();
}


bool KcThemedPlotImpl_::minorColorNeeded(unsigned idx) const
{
    return plot_.plottable(idx)->minorColorNeeded();
}


void KcThemedPlotImpl_::applyMajorColors(unsigned idx, const std::vector<color4f>& majors)
{
    plot_.plottable(idx)->setMajorColors(majors);
}


void KcThemedPlotImpl_::applyMinorColor(unsigned idx, const color4f& minor)
{
    plot_.plottable(idx)->setMinorColor(minor);
}


KcThemedPlotImpl_::KeLegendPlacement KcThemedPlotImpl_::legendPlacement()
{
	return k_place_inner;
}


void KcThemedPlotImpl_::setLegendPlacement(KeLegendPlacement)
{

}


int KcThemedPlotImpl_::legendAlignment()
{
	return 0;
}


void KcThemedPlotImpl_::setLegendAlignment(int)
{

}


KcThemedPlotImpl_::KeLegendArrangement KcThemedPlotImpl_::legendArrangement()
{
	return k_arrange_row;
}


void KcThemedPlotImpl_::setLegendArrangement(KeLegendArrangement)
{

}


std::pair<int, int> KcThemedPlotImpl_::legendSpacing()
{
	return { 0,0 };
}


void KcThemedPlotImpl_::setLegendSpacing(int xspacing, int yspacing)
{

}

