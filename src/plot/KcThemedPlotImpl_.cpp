#include "KcThemedPlotImpl_.h"
#include "KvPlot.h"


KcThemedPlotImpl_::KcThemedPlotImpl_(KvPlot& plot)
    : plot_(plot)
{

}


KpBrush KcThemedPlotImpl_::fill(int level) const
{
	//if (level == k_plot)
		return plot_.background();
	//else if (level == k_axis)
	//	return qcp_->axisRect()->backgroundBrush();
	//else if (level == k_legend)
	//	return qcp_->legend->brush();
	//else
	//	assert(false);

	return KpBrush::null(); // make compiler happy
}


void KcThemedPlotImpl_::applyFill(int level, const KpBrush& b)
{
	plot_.background() = b;
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
	return KtMargins<float>();
}


void KcThemedPlotImpl_::applyMargins(int level, const KtMargins<float>&)
{

}


bool KcThemedPlotImpl_::visible(int level) const
{
	return true;
}


void KcThemedPlotImpl_::applyVisible(int level, bool b)
{

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

