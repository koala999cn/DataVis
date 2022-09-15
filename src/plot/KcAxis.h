#pragma once
#include <vector>
#include <string>
#include <memory>
#include "KvRenderable.h"
#include "KtVector3.h"
#include "KtVector4.h"
#include "KvTicker.h"


// 坐标轴（单轴）实现
// 坐标轴由4类元素构成：1.baseline, 2.ticks(major & minor), 3.labels, 4.title

class KcAxis : public KvRenderable
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


	KcAxis();

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

	bool tickShowBothSide() const { return tickShowBothSide_; }

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

	void draw(KglPaint*) const override;

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

	double baselineWidth() const { return baselineWidth_; }
	void setBaselineWidth(double width) { baselineWidth_ = width; }

	// major ticks
	double tickWidth() const { return tickWidth_; }
	void setTickWidth(double width) { tickWidth_ = width; }
	double tickLength() const { return tickLength_; }
	void setTickLength(double len) { tickLength_ = len; }

	// minor ticks
	double subtickWidth() const { return subtickWidth_; }
	void setSubtickWidth(double width) { subtickWidth_ = width; }
	double subtickLength() const { return subtickLength_; }
	void setSubtickLength(double len) { subtickLength_ = len; }

	// tick长度以rlen为参考值取百分数
	// 如，设tick为5，rlen为300，则绘制tick的实际长度为300*5%，即15
	// rlen通常置为坐标系aabb的对角线长度
	void setRefLength(double rlen) { refLength_ = rlen; }

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

	std::shared_ptr<KvTicker> ticker() const;
	void setTicker(std::shared_ptr<KvTicker> tic);

	// 根据tick的数值，返回tick在坐标轴上的的3维坐标
	vec3 tickPos(double val) const;

private:
	void drawTicks_(KglPaint*) const; // 绘制所有刻度
	void drawTick_(KglPaint*, const vec3& anchor, double length) const; // 绘制单条刻度线，兼容主刻度与副刻度
	static int labelAlignment_(const vec3& orient); // 根据tick的orientation判定label的alignment

private:
	std::string title_;
	std::vector<std::string> labels_; // tick labels
	double lower_, upper_; // range
	bool showBaseline_, showTick_, showSubtick_, showTitle_, showLabel_;

	double baselineWidth_;
	double tickWidth_, tickLength_;
	double subtickWidth_, subtickLength_;
	double labelPadding_; // 刻度label距离刻度线的距离
	mutable double refLength_; // 标准参考长度. tickLength_, subtickLength_, labelPadding_均为其相对值
	                           // 一般取AABB的对角线长度

	vec4 baselineColor_;
	vec4 tickColor_, subtickColor_;
	vec4 labelColor_, titleColor_;

	//QFont labelFont_, titleFont_;

	vec3 start_, end_;
	vec3 tickOrient_;
	bool tickShowBothSide_;

	std::shared_ptr<KvTicker> ticker_;
};
