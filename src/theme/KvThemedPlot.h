#pragma once
#include <functional>
#include <QMargins>
#include <QBrush>

class QPen;
class QColor;
class QFont;


// 一个可应用主题的plotting接口

class KvThemedPlot
{
public:

	// level有两个方面：一是方位，二是属性
	enum KeLevel
	{
		/// 方位

		k_plot	= 0x1,
		k_axis	= 0x2,
		k_grid	= 0x4,
		k_legend	= 0x8,

		k_left = 0x10,
		k_right = 0x20,
		k_top = 0x40,
		k_bottom = 0x80,
		k_x = k_top | k_bottom,
		k_y = k_left | k_right,
		k_xy	= k_x | k_y,

		k_axis_xy	= k_axis | k_xy,
		k_grid_xy	= k_grid | k_xy,


		/// 属性

		k_title	= 0x100,
		k_label	= 0x200,
		k_text = k_title | k_label,
		
		k_axis_baseline	= 0x400,
		k_axis_tick_major	= 0x800,
		k_axis_tick_minor	= 0x1000,
		k_axis_tick		= k_axis_tick_major | k_axis_tick_minor,
		k_axis_line	= k_axis_baseline | k_axis_tick,
		
		k_grid_major	= 0x2000,
		k_grid_minor	= 0x4000,
		k_grid_zeroline	= 0x8000,
		k_grid_line	= k_grid_major | k_grid_minor | k_grid_zeroline,

		k_line	= k_axis_line | k_grid_line,

		/// 组合

		k_plot_title		= k_plot | k_title,
		k_plot_label		= k_plot | k_label,
		k_plot_text			= k_plot | k_text,
		k_plot_all			= k_plot_text,


		k_axis_title = k_axis | k_title,
		k_axis_label = k_axis | k_label,
		k_axis_text = k_axis | k_text,

		k_axis_title_all = k_xy | k_axis_title,
		k_axis_label_all = k_xy | k_axis_label,
		k_axis_text_all = k_xy | k_axis_text,

		k_axis_baseline_all = k_axis_xy | k_axis_baseline,
		k_axis_tick_major_all = k_axis_xy | k_axis_tick_major,
		k_axis_tick_minor_all = k_axis_xy | k_axis_tick_minor,
		k_axis_tick_all = k_axis_tick_major_all | k_axis_tick_minor_all,
		k_axis_line_all	= k_axis_baseline_all | k_axis_tick_all,

		k_axis_all			= k_axis_text | k_axis_line_all,


		k_grid_major_all = k_axis_xy | k_grid_major,
		k_grid_minor_all = k_axis_xy | k_grid_minor,
		k_grid_zeroline_all = k_axis_xy | k_grid_zeroline,
		k_grid_line_all			= k_axis_xy | k_grid_line,
		k_grid_all				= k_grid_line_all,

		k_legend_title		= k_legend | k_title,
		k_legend_label		= k_legend | k_label,
		k_legend_text		= k_legend | k_text,
		k_legend_all			= k_legend_text,


		k_line_all			= k_axis_line_all | k_grid_line_all,
		k_title_all			= k_axis_title_all | k_legend_title | k_plot_title,
		k_label_all			= k_axis_label_all | k_legend_label | k_plot_label,
		k_text_all			= k_axis_text_all | k_legend_text | k_plot_text,

		k_all				= k_line_all | k_text_all
	};
	
	enum KeTickSide
	{
		k_tick_none		= 0x00,
		k_tick_inside	= 0x01,
		k_tick_outside	= 0x02,
		k_tick_bothside	= k_tick_inside | k_tick_outside
	};


	virtual ~KvThemedPlot() {}

	virtual QBrush background() const = 0;
	virtual void setBackground(const QBrush&) = 0;

	virtual QBrush axisBackground() const = 0;
	virtual void setAxisBackground(const QBrush&) = 0;

	virtual void applyLine(int level, std::function<QPen(const QPen&)> op) = 0;
	virtual void applyText(int level, std::function<QFont(const QFont&)> op) = 0;
	virtual void applyTextColor(int level, std::function<QColor(const QColor&)> op) = 0;

	virtual void setTickLength(int level, KeTickSide side, int len) = 0;

	virtual unsigned numPlots() const = 0;
	virtual void applyPalette(unsigned plotIdx, const QColor& major, const QColor& minor) = 0;

	virtual void setVisible(int level, bool b) = 0; // level其一可见，则返回true

	virtual QMargins margins() const = 0;
	virtual void setMargins(const QMargins&) = 0;
};

