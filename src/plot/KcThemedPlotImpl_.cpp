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
	if (level & k_plot)
		plot_.background() = b;
	else if (level & k_axis) {
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
	if (level == k_plot) 
		return plot_.margins();

	return KtMargins<float>();
}


void KcThemedPlotImpl_::applyMargins(int level, const KtMargins<float>& margins)
{
	if (level == k_plot) 
		plot_.setMargins(margins);
}


bool KcThemedPlotImpl_::visible(int level) const
{
	return true;
}


void KcThemedPlotImpl_::applyVisible(int level, bool b)
{
	if (level & k_axis) {
		forAxis_(level, [level, b](KcAxis& axis) {

			if ((level & k_axis_all) == k_axis_all) {
				axis.visible() = b;
			}
			else {
				if (level & k_baseline)
					axis.showBaseline() = b;

				if (level & k_majorline)
					axis.showTick() = b;

				if (level & k_minorline)
					axis.showSubtick() = b;

				if (level & k_label)
					axis.showLabel() = b;

				if (level & k_title)
					axis.showTitle() = b;
			}
			return true;
			});
	}
	
	if (level & k_grid) {
		forPlane_(level, [level, b](KcCoordPlane& plane) {

			if ((level & k_grid_all) == k_grid_all) {
				plane.visible() = b;
			}
			else {
				if (level & k_majorline)
					plane.majorVisible() = b;

				if (level & k_minorline)
					plane.minorVisible() = b;
			}

			return true;
			});
	}
}


void KcThemedPlotImpl_::applyLine(int level, std::function<KpPen(const KpPen&)> op)
{
	if (level & k_axis) {
		forAxis_(level, [level, op](KcAxis& axis) {
			if (level & k_baseline)
				axis.baselineContext() = op(axis.baselineContext());
			if (level & k_majorline)
				axis.tickContext() = op(axis.tickContext());
			if (level & k_minorline)
				axis.subtickContext() = op(axis.subtickContext());
			return true;
			});
	}

	if (level & k_grid) {
		forPlane_(level, [level, op](KcCoordPlane& plane) {
			if (level & k_majorline)
				plane.majorLine() = op(plane.majorLine());
			if (level & k_minorline)
				plane.minorLine() = op(plane.minorLine());
			return true;
			});
	}
}


void KcThemedPlotImpl_::applyText(int level, std::function<KpFont(const KpFont&)> op)
{

}


void KcThemedPlotImpl_::applyTextColor(int level, std::function<color4f(const color4f&)> op)
{
	if (level & k_axis) {
		forAxis_(level, [level, op](KcAxis& axis) {
			if (level & k_label)
				axis.labelContext().color = op(axis.labelContext().color);
			if (level & k_title)
				axis.titleContext().color = op(axis.titleContext().color);
			return true;
			});
	}
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
    return plot_.plottableAt(idx)->majorColorsNeeded();
}


bool KcThemedPlotImpl_::minorColorNeeded(unsigned idx) const
{
    return plot_.plottableAt(idx)->minorColorNeeded();
}


void KcThemedPlotImpl_::applyMajorColors(unsigned idx, const std::vector<color4f>& majors)
{
    plot_.plottableAt(idx)->setMajorColors(majors);
}


void KcThemedPlotImpl_::applyMinorColor(unsigned idx, const color4f& minor)
{
    plot_.plottableAt(idx)->setMinorColor(minor);
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


void KcThemedPlotImpl_::forAxis_(int level, std::function<bool(KcAxis&)> op)
{
	static constexpr int typeMap[] = {
		k_near_left,
		k_near_right,
		k_near_bottom,
		k_near_top,
		k_far_left,
		k_far_right,
		k_far_bottom,
		k_far_top,
		k_floor_left,
		k_floor_right,
		k_ceil_left,
		k_ceil_right 
    };

	plot_.coord().forAxis([level, op](KcAxis& axis) {
		if (!(level & typeMap[axis.type()]))
			return true; // 类型不匹配，跳过该axis

		return op(axis);
		});
}


void KcThemedPlotImpl_::forPlane_(int level, std::function<bool(KcCoordPlane&)> op)
{
	static constexpr int typeMap[] = {
		k_plane_back,
		k_plane_front,
		k_plane_left,
		k_plane_right,
		k_plane_ceil,
		k_plane_floor
	};

	plot_.coord().forPlane([level, op](KcCoordPlane& plane) {
		if (!(level & typeMap[plane.type()]))
			return true; // 类型不匹配，跳过该plane

		return op(plane);
		});
}