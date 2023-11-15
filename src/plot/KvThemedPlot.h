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
		k_near_left = k_left,
		k_near_right = k_right,
		k_near_top = k_top,
		k_near_bottom = k_bottom,
		k_far_left = 0x100,
		k_far_right = 0x200,
		k_far_top = 0x400,
		k_far_bottom = 0x800,
		k_floor_left = 0x1000,
		k_floor_right = 0x2000,
		k_ceil_left = 0x4000,
		k_ceil_right = 0x8000,

		k_all_axis_direction = 0xfff0,

		k_plane_front = 0x10000,
		k_plane_back = 0x20000,
		k_plane_left = 0x40000,
		k_plane_right = 0x80000,
		k_plane_floor = 0x100000,
		k_plane_ceil = 0x200000,

		k_all_plane_direction = 0x3f0000,

		/// 属性

		k_title				= 0x01000000,
		k_label				= 0x02000000,
		k_text				= k_title | k_label,
		
		k_baseline			= 0x04000000,
		k_majorline			= 0x08000000,
		k_minorline			= 0x10000000,
		k_line				= k_baseline | k_majorline | k_minorline,

		/// 组合

		k_plot_title			= k_plot | k_title,
		k_plot_label			= k_plot | k_label,
		k_plot_text				= k_plot | k_text,
		k_plot_all				= k_plot_text,


		k_axis_title			= k_axis | k_title,
		k_axis_label			= k_axis | k_label,
		k_axis_text				= k_axis | k_text,

		k_axis_baseline		    = k_axis | k_baseline,
		k_axis_major_tick		= k_axis | k_majorline,
		k_axis_minor_tick		= k_axis | k_minorline,
		k_axis_tick				= k_axis_major_tick | k_axis_minor_tick,
		k_axis_line				= k_axis | k_line,

		k_axis_all				= k_axis_text | k_axis_line,

		k_all_axis_text			= k_all_axis_direction | k_axis_text,
		k_all_axis_line			= k_all_axis_direction | k_axis_line,

		k_all_axis_all			= k_all_axis_text | k_all_axis_line,

		k_grid_zeroline			= k_grid | k_baseline,
		k_grid_major			= k_grid | k_majorline,
		k_grid_minor			= k_grid | k_minorline,

		k_grid_line				= k_grid | k_line,
		k_grid_all				= k_grid_line,

		k_all_grid_line			= k_all_plane_direction | k_grid_line,
		k_all_grid_all			= k_all_plane_direction | k_grid_all,

		k_legend_title			= k_legend | k_title,
		k_legend_label			= k_legend | k_label,
		k_legend_text			= k_legend | k_text,
		k_legend_all			= k_legend_text,

		k_all_line				= k_all_axis_line | k_all_grid_line,
		k_all_text				= k_all_axis_text | k_legend_text | k_plot_text,

		k_all					= k_all_line | k_all_text
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

