#pragma once
#include <vector>
#include <string>
#include <memory>
#include "KvRenderable.h"
#include "KvScaler.h"
#include "KtColor.h"
#include "KtVector3.h"
#include "KtMargins.h"
#include "KpContext.h"
#include "layout/KvLayoutElement.h"


// 坐标轴（单轴）实现
// 坐标轴由4类元素构成：1.baseline, 2.ticks(major & minor), 3.labels, 4.title

class KcAxis : public KvRenderable, public KvLayoutElement
{
	using KvRenderable::float_t;

public:
	using point2 = KtPoint<float_t, 2>;
	using point3 = KtPoint<float_t, 3>;
	using vec3 = KtVector3<float_t>;

	// 3d坐标系中的12根坐标轴
	enum KeAxisType
	{
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
		k_ceil_right,

		k_left = k_near_left,
		k_right = k_near_right,
		k_bottom = k_near_bottom,
		k_top = k_near_top
	};

	enum KeTickOrient
	{
		k_x, k_neg_x, k_bi_x,
		k_y, k_neg_y, k_bi_y,
		k_z, k_neg_z, k_bi_z
	};

	struct KpTickContext : public KpPen
	{
		double length;

		KpPen& operator=(const KpPen& pen) {
			style = pen.style;
			width = pen.width;
			color = pen.color;
			return *this;
		}
	};

	KcAxis(KeAxisType type, int dim, bool main);

	KeAxisType type() const { return type_; }
	void setType(KeAxisType t) { type_ = t; }

	const point3& start() const { return start_; }
	void setStart(const point3& v) { start_ = v; }
	void setStart(double x, double y, double z) { start_ = point3(x, y, z); }

	const point3& end() const { return end_; }
	void setEnd(const point3& v) { end_ = v; }
	void setEnd(double x, double y, double z) { end_ = point3(x, y, z); }

	void setExtend(const point3& st, const point3& ed) {
		start_ = st, end_ = ed;
	}

	const vec3& tickOrient() const { return tickOrient_; }
	vec3& tickOrient() { return tickOrient_; }

	const vec3& labelOrient() const { return labelOrient_; }
	vec3& labelOrient() { return labelOrient_; }

	bool tickBothSide() const { return tickBothSide_; }
	bool& tickBothSide() { return tickBothSide_; }

	/// range 

	double lower() const { return lower_; }
	double& lower() { return lower_; }
	double upper() const { return upper_; }
	double& upper() { return upper_; }

	void setRange(double l, double u) {
		lower_ = l, upper_ = u;
	}

	double length() const {
		return upper_ - lower_; // == (end - start).length ? 
	}

	bool showBaseline() const { return showBaseline_; }
	bool& showBaseline() { return showBaseline_; }

	bool showTick() const { return showTick_; }
	bool& showTick() { return showTick_; }

	bool showSubtick() const { return showSubtick_; }
	bool& showSubtick() { return showSubtick_; }

	bool showTitle() const { return showTitle_; }
	bool& showTitle() { return showTitle_; }

	bool showLabel() const { return showLabel_; }
	bool& showLabel() { return showLabel_; }

	void showAll() {
		showBaseline_ = true;
		showTick_ = true, showSubtick_ = true;
		showTitle_ = true, showLabel_ = true;
	}

	const std::string& title() const { return title_; }
	std::string& title() { return title_; }

	const std::vector<std::string>& labels() const { return labels_; }
	void setLabels(const std::vector<std::string>& ls) { labels_ = ls; }

	const KpPen& baselineContext() const { return baselineCxt_; }
	KpPen& baselineContext() { return baselineCxt_; }

	const KpTickContext& tickContext() const { return tickCxt_; }
	KpTickContext& tickContext() { return tickCxt_; }

	const KpTickContext& subtickContext() const { return subtickCxt_; }
	KpTickContext& subtickContext() { return subtickCxt_; }

	/// colors

	const color4f& titleColor() const { return titleColor_; }
	color4f& titleColor() { return titleColor_; }

	const color4f& labelColor() const { return labelColor_; }
	color4f& labelColor() { return labelColor_; }

	/// fonts

	//QFont labelFont() const { return labelFont_; }
	//void setLabelFont(QFont font) { labelFont_ = font; }

	//QFont titleFont() const { return titleFont_; }
	//void setTitleFont(QFont font) { titleFont_ = font; }

	std::shared_ptr<KvScaler> scaler() const;
	void setScaler(std::shared_ptr<KvScaler> scale);

	aabb_t boundingBox() const override;

	void draw(KvPaint*) const override;

	// 根据tick的数值，返回tick在坐标轴上的的3维坐标（世界坐标）
	point3 tickPos(double val) const;

	// 返回当前axis在屏幕坐标所占的尺寸（像素大小）
	KtMargins<float_t> calcMargins(KvPaint* paint) const;

	int dim() const { return dim_; }

	bool main() const { return main_; }

	bool inversed() const { return inv_; }
	void setInversed(bool inv) { inv_ = inv; }

private:
	void drawTicks_(KvPaint*) const; // 绘制所有刻度
	void drawTick_(KvPaint*, const point3& anchor, double length) const; // 绘制单条刻度线，兼容主刻度与副刻度
	
	int labelAlignment_(KvPaint* paint, bool toggleTopBottom) const; // 根据label的orientation判定label的alignment
	bool tickAndLabelInSameSide_() const; // 判断tick与tick-label是否位于坐标轴的同侧
	aabb_t textBox_(KvPaint*, const point3& anchor, const std::string& text) const;

	size_t calcSize_(void* cxt) const final;

private:
	KeAxisType type_;
	std::string title_;
	std::vector<std::string> labels_; // tick labels
	double lower_, upper_; // range
	bool showBaseline_, showTick_, showSubtick_, showTitle_, showLabel_;

	KpPen baselineCxt_;
	KpTickContext tickCxt_, subtickCxt_;

	double labelPadding_{ 2 }; 
	double titlePadding_{ 2 };

	color4f labelColor_{ 0, 0, 0, 1 }, titleColor_{ 0, 0, 0, 1 };

	//QFont labelFont_, titleFont_;

	point3 start_, end_;
	vec3 tickOrient_;
	vec3 labelOrient_;
	bool tickBothSide_{ false };

	std::shared_ptr<KvScaler> scaler_;

	int dim_; // 0表示x轴，1表示y轴，2表示z轴，-1表示数据轴（用来显示colorbar）
	bool main_{ true }; // 是否主坐标轴
	bool inv_{ false }; // 是否反转坐标轴
};
