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

	enum KeLevel
	{
		k_all				= 0xffffffff,

		k_plot_line			= 0x10000000,
		k_plot_title		= 0x20000000,
		k_plot_label		= 0x40000000,
		k_plot_text			= k_plot_title | k_plot_label,

		k_axis				= 0x0000ffff,

		k_axis_left			= 0x00000ff1,
		k_axis_right		= 0x00000ff2,
		k_axis_top			= 0x00000ff4,
		k_axis_bottom		= 0x00000ff8,
		k_axis_x			= k_axis_top | k_axis_bottom,
		k_axis_y			= k_axis_left | k_axis_right,

		k_axis_baseline		= 0x0000001f,
		k_axis_tick_major	= 0x0000002f,
		k_axis_tick_minor	= 0x0000004f,
		k_axis_tick			= k_axis_tick_major | k_axis_tick_minor,
		k_axis_line			= k_axis_baseline | k_axis_tick,

		k_axis_title		= 0x0000010f,
		k_axis_label		= 0x0000020f,
		k_axis_text			= k_axis_title | k_axis_label,

		
		k_grid				= 0x00ff0000,

		k_grid_left			= 0x00f10000,
		k_grid_right		= 0x00f20000,
		k_grid_top			= 0x00f40000,
		k_grid_bottom		= 0x00f80000,
		k_grid_x			= k_grid_top | k_grid_bottom,
		k_grid_y			= k_grid_left | k_grid_right,
		k_grid_line			= k_grid_x | k_grid_y,

		k_grid_major		= 0x001f0000,
		k_grid_minor		= 0x002f0000,
		k_grid_zeroline		= 0x004f0000,

		k_legend			= 0x0f000000,
		k_legend_title		= 0x01000000,
		k_legend_label		= 0x02000000,
		k_legend_text		= k_legend_title | k_legend_label,

		k_all_line			= k_axis_line | k_grid_line | k_plot_line,
		k_all_title			= k_axis_title | k_legend_title | k_plot_title,
		k_all_label			= k_axis_label | k_legend_label | k_plot_label,
		k_all_text			= k_plot_title | k_plot_label | k_plot_text
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

	virtual void setLineVisible(int level, bool b) = 0; // level其一可见，则返回true
	virtual bool lineVisible(int level) const = 0;

	virtual void setTextVisible(int level, bool b) = 0;
	virtual bool textVisible(int level) const = 0;

	virtual QMargins margins() const = 0;
	virtual void setMargins(const QMargins&) = 0;
};

