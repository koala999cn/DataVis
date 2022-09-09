#pragma once
#include <vector>
#include <string>
#include "KtVector3.h"
#include "KtVector4.h"

// 坐标轴（单轴）实现
// 坐标轴由4类元素构成：1.baseline, 2.ticks(major & minor), 3.labels, 4.title

class KgAxis
{
	using vec3 = KtVector3<double>;
	using vec4 = KtVector4<double>;

public:

	enum KeTickOrient
	{
		k_x, k_neg_x, k_bi_x,
		k_y, k_neg_y, k_bi_y,
		k_z, k_neg_z, k_bi_z
	};


	KgAxis();

	const vec3& start() const { return start_; }
	void setStart(const vec3& v) { start_ = v; }
	void setStart(double x, double y, double z) { start_ = vec3(x, y, z); }

	const vec3& end() const { return end_; }
	void setEnd(const vec3& v) { end_ = v; }
	void setEnd(double x, double y, double z) { end_ = vec3(x, y, z); }

	const vec3& tickOrient() const { return tickOrient_; }

	void setTickOrient(const vec3& v, bool bothSide) { 
		tickOrient_ = v; 
		tickShowBothSide_ = bothSide;
	}

	void setTickOrient(KeTickOrient orient);

	/// range 

	double lower() const { return lower_; }
	double upper() const { return upper_; }
	std::pair<double, double> range() const { return { lower_, upper_ }; }
	void setLower(double lower) { lower_ = lower; }
	void setUpper(double upper) { upper_ = upper; }
	void setRange(double lower, double upper) { lower_ = lower, upper_ = upper; }

	double length() const {
		return upper_ - lower_; // == (end - start).length ? 
	}

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
	void setLabels(const std::vector<std::string>& ls) { labels_ = ls; }

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


	/// colors

	vec4 baselineColor() const { return baselineColor_; }
	void setBaselineColor(vec4 clr) { baselineColor_ = clr; }

	vec4 tickColor() const { return tickColor_; }
	void setTickColor(vec4 clr) { tickColor_ = clr; }

	vec4 subtickColor() const { return subtickColor_; }
	void setSubtickColor(vec4 clr) { subtickColor_ = clr; }

	vec4 labelColor() const { return labelColor_; }
	void setLabelColor(vec4 clr) { labelColor_ = clr; }

	vec4 titleColor() const { return titleColor_; }
	void setTitleColor(vec4 clr) { titleColor_ = clr; }

	/// fonts

	//QFont labelFont() const { return labelFont_; }
	//void setLabelFont(QFont font) { labelFont_ = font; }

	//QFont titleFont() const { return titleFont_; }
	//void setTitleFont(QFont font) { titleFont_ = font; }

private:
	std::string title_;
	std::vector<std::string> labels_; // tick labels
	double lower_, upper_; // range
	bool visible_;
	bool showBaseline_, showTick_, showSubtick_, showTitle_, showLabel_;

	double baselineSize_;
	double tickSize_, tickLength_;
	double subtickSize_, subtickLength_;

	vec4 baselineColor_;
	vec4 tickColor_, subtickColor_;
	vec4 labelColor_, titleColor_;

	//QFont labelFont_, titleFont_;

	vec3 start_, end_;
	vec3 tickOrient_;
	bool tickShowBothSide_;
};
