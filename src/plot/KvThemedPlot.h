#pragma once
#include <functional>
#include "KpContext.h"
#include "KtMargins.h"


// 一个可应用主题的plotting接口

class KvThemedPlot
{
public:

	// level有两个方面：一是方位，二是属性
	enum KeLevel
	{
		/// 方位

		k_plot		= 0x1,
		k_axis		= 0x2,
		k_grid		= 0x4,
		k_legend	= 0x8,

		k_element	= k_plot | k_axis | k_grid | k_legend,

		k_left		= 0x10,
		k_right		= 0x20,
		k_top		= 0x40,
		k_bottom	= 0x80,
		k_x			= k_top | k_bottom,
		k_y			= k_left | k_right,
		k_xy		= k_x | k_y,

		k_axis_xy	= k_axis | k_xy,
		k_grid_xy	= k_grid | k_xy,



		/// 属性

		k_title				= 0x100,
		k_label				= 0x200,
		k_text				= k_title | k_label,
		
		k_axis_baseline		= 0x400,
		k_axis_tick_major	= 0x800,
		k_axis_tick_minor	= 0x1000,
		k_axis_tick			= k_axis_tick_major | k_axis_tick_minor,
		k_axis_line			= k_axis_baseline | k_axis_tick,
		
		k_grid_major		= 0x2000,
		k_grid_minor		= 0x4000,
		k_grid_zeroline		= 0x8000,
		k_grid_line			= k_grid_major | k_grid_minor | k_grid_zeroline,

		k_line				= k_axis_line | k_grid_line,

		/// 组合

		k_plot_title			= k_plot | k_title,
		k_plot_label			= k_plot | k_label,
		k_plot_text				= k_plot | k_text,
		k_plot_all				= k_plot_text,


		k_axis_title			= k_axis | k_title,
		k_axis_label			= k_axis | k_label,
		k_axis_text				= k_axis | k_text,

		k_axis_title_left		= k_left | k_axis_title,
		k_axis_label_left		= k_left | k_axis_label,
		k_axis_text_left		= k_left | k_axis_text,

		k_axis_title_all		= k_xy | k_axis_title,
		k_axis_label_all		= k_xy | k_axis_label,
		k_axis_text_all			= k_xy | k_axis_text,

		k_axis_baseline_all		= k_axis_xy | k_axis_baseline,
		k_axis_tick_major_all	= k_axis_xy | k_axis_tick_major,
		k_axis_tick_minor_all	= k_axis_xy | k_axis_tick_minor,
		k_axis_tick_all			= k_axis_tick_major_all | k_axis_tick_minor_all,
		k_axis_line_all			= k_axis_baseline_all | k_axis_tick_all,

		k_axis_all				= k_axis_text | k_axis_line_all,

		k_grid_major_all		= k_grid_xy | k_grid_major,
		k_grid_minor_all		= k_grid_xy | k_grid_minor,
		k_grid_zeroline_all		= k_grid_xy | k_grid_zeroline,
		k_grid_line_all			= k_grid_xy | k_grid_line,
		k_grid_all				= k_grid_line_all,

		k_legend_title			= k_legend | k_title,
		k_legend_label			= k_legend | k_label,
		k_legend_text			= k_legend | k_text,
		k_legend_all			= k_legend_text,


		k_line_all				= k_axis_line_all | k_grid_line_all,
		k_title_all				= k_axis_title_all | k_legend_title | k_plot_title,
		k_label_all				= k_axis_label_all | k_legend_label | k_plot_label,
		k_text_all				= k_axis_text_all | k_legend_text | k_plot_text,

		k_all					= k_line_all | k_text_all
	};
	
	enum KeTickSide
	{
		k_tick_none		= 0x00,
		k_tick_inside	= 0x01,
		k_tick_outside	= 0x02,
		k_tick_bothside	= k_tick_inside | k_tick_outside
	};

	enum KeLegendPlacement
	{
		k_place_inner,
		k_place_outter
	};

	enum KeLegendAlignment
	{
		k_align_auto = 0x00,
		k_align_left = 0x01,
		k_align_right = 0x02,
		k_align_top = 0x04,
		k_align_bottom = 0x08,
		k_align_center = 0x10,

		// 当KeLegendPlacement为outter模式时，使用以下2个枚举量区分水平和纵向优先级
		// 例如，如果k_align_left和k_align_top均被设置，则
		//   -- 当k_align_vert_first有效时，legend位于axis-rect的顶端位置靠左对齐；
		//   -- 当k_align_horz_first有效时，legend位于axis-rect的左端位置靠上对齐。
		k_align_vert_first = 0x20,
		k_align_horz_first = 0x40
	};

	enum KeLegendArrangement
	{
		k_arrange_row,
		k_arrange_column
	};

	virtual ~KvThemedPlot() {}

	// 填充(KpBrush)
	virtual KpBrush fill(int level) const = 0;
	virtual void applyFill(int level, const KpBrush&) = 0;

	// 边框(KpPen)
	virtual KpPen border(int level) const = 0;
	virtual void applyBorder(int level, const KpPen&) = 0;

	// 留白（KtMargins<float>）
	virtual KtMargins<float> margins(int level) const = 0;
	virtual void applyMargins(int level, const KtMargins<float>&) = 0;

	// 可见性(bool)
	virtual bool visible(int level) const = 0;
	virtual void applyVisible(int level, bool b) = 0;

	// 线条
	virtual void applyLine(int level, std::function<KpPen(const KpPen&)> op) = 0;

	// 字体
	virtual void applyText(int level, std::function<KpFont(const KpFont&)> op) = 0;

	// 文字颜色
	virtual void applyTextColor(int level, std::function<color4f(const color4f&)> op) = 0;

	// 坐标轴刻度
	virtual void setTickLength(int level, KeTickSide side, int len) = 0;

	virtual unsigned plottableCount() const = 0;
	virtual unsigned majorColorsNeeded(unsigned plottableIdx) const = 0;
	virtual bool minorColorNeeded(unsigned plottableIdx) const = 0;
	virtual void applyMajorColors(unsigned plottableIdx, const std::vector<color4f>& majors) = 0;
	virtual void applyMinorColor(unsigned plottableIdx, const color4f& minor) = 0;


	// legend相关操作

	virtual KeLegendPlacement legendPlacement() = 0;
	virtual void setLegendPlacement(KeLegendPlacement) = 0;

	virtual int legendAlignment() = 0;
	virtual void setLegendAlignment(int) = 0;

	virtual KeLegendArrangement legendArrangement() = 0;
	virtual void setLegendArrangement(KeLegendArrangement) = 0;

	virtual std::pair<int, int> legendSpacing() = 0;
	virtual void setLegendSpacing(int xspacing, int yspacing) = 0;

	// KeLevel有关的帮助函数

	static int enterLevel(int curLevel, int into);
};

