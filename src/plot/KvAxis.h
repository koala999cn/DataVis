#pragma once
#include <vector>
#include <string>
#include <QColor>
#include <QFont>


// 坐标轴（单轴）基类
// 坐标轴由4类元素构成：1.baseline, 2.ticks(major & minor), 3.labels, 4.title

class KvAxis
{
public:
	enum KeType
	{
		k_x,
		k_y,
		k_z
	};

	KvAxis(KeType type);

	KeType type() const { return type_; }

	/// range 

	double lower() const { return lower_; }
	double upper() const { return upper_; }
	std::pair<double, double> range() const { return { lower_, upper_ }; }
	void setLower(double lower) { lower_ = lower; }
	void setUpper(double upper) { upper_ = upper; }
	void setRange(double lower, double upper) { lower_ = lower, upper_ = upper; }

	bool visible() const { return visible_; }
	void setVisible(bool b) { visible_ = b; }

	bool showBaseline() const { return showBaseline_; }
	void setShowBaseline(bool b) { showBaseline_ = b; }

	bool showTick() const { return showTick_; }
	void setShowTick(bool b) { showTick_ = b; }

	bool showSubtick() const { return showSubtick_; }
	void setShowSubtick(bool b) { showSubtick_ = b; }

	bool showTitle() const { return showTitle_; }
	void setShowTitle(bool b) { showTitle_ = b; }

	bool showLabel() const { return showLabel_; }
	void setShowLabel(bool b) { showLabel_ = b; }

	void showAll() {
		showBaseline_ = true;
		showTick_ = true, showSubtick_ = true;
		showTitle_ = true, showLabel_ = true;
	}

	const std::string& title() const { return title_; }
	void setTitle(const std::string& title) { title_ = title; }

	const std::vector<std::string>& labels() const { return labels_; }
	void setLabels(const std::vector<std::string>& lables) { labels_ = labels; }

	enum KeTickSide
	{
		k_inner_side,
		k_outter_side,
		k_both_side
	};

	double baselineSize() const { return baselineSize_; }
	void setBaselineSize(double size) { baselineSize_ = size; }

	// major ticks
	double tickSize() const { return tickSize_; }
	void setTickSize(double size) { tickSize_ = size; }
	double tickLength() const { return tickLength_; }
	void setTickLength(double len) { tickLength_ = len; }

	// minor ticks
	double subtickSize() const { return subtickSize_; }
	void setSubtickSize(double size) { subtickSize_ = size; }
	double subtickLength() const { return subtickLength_; }
	void setSubtickLength(double len) { subtickLength_ = len; }

	KeTickSide tickSide() const { return tickSide_; }
	void setTickSide(KeTickSide side) { tickSide_ = side; }

	/// colors

	QColor baselineColor() const { return baselineColor_; }
	void setBaselineColor(QColor clr) { baselineColor_ = clr; }

	QColor tickColor() const { return tickColor_; }
	void setTickColor(QColor clr) { tickColor_ = clr; }

	QColor subtickColor() const { return subtickColor_; }
	void setSubtickColor(QColor clr) { subtickColor_ = clr; }

	QColor labelColor() const { return labelColor_; }
	void setLabelColor(QColor clr) { labelColor_ = clr; }

	QColor titleColor() const { return titleColor_; }
	void setTitleColor(QColor clr) { titleColor_ = clr; }

	/// fonts

	QFont labelFont() const { return labelFont_; }
	void setLabelFont(Font font) { labelFont_ = font; }

	QFont titleFont() const { return titleFont_; }
	void setTitleFont(Font font) { titleFont_ = font; }

private:
	KeType type_; // type
	std::string title_;
	std::vector<std::string> labels_; // tick labels
	double lower_, upper_; // range
	bool visible_;
	bool showBaseline_, showTick_, showSubtick_, showTitle_, showLabel_;

	double baselineSize_;
	double tickSize_, tickLength_;
	double subtickSize_, subtickLength_;
	KeTickSide tickSide_;

	QColor baselineColor_;
	QColor tickColor_, subtickColor_;
	QColor labelColor_, titleColor_;

	QFont labelFont_, titleFont_;
};
