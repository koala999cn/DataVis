#pragma once
#include <vector>
#include <string>
#include <memory>
#include "KvRenderable.h"
#include "KvTicker.h"
#include "KtColor.h"
#include "KtVector3.h"
#include "KpContext.h"
#include "KtMargins.h"
#include "layout/KvLayoutElement.h"


// 坐标轴（单轴）实现
// 坐标轴由4类元素构成：1.baseline, 2.ticks(major & minor), 3.labels, 4.title

class KcAxis : public KvRenderable, public KvLayoutElement
{
	using KvRenderable::float_t;
	using KvLayoutElement::rect_t;

public:
	using point2 = KtPoint<float_t, 2>;
	using point3 = KtPoint<float_t, 3>;
	using vec3 = KtVector3<float_t>;

	// 3d坐标系中的12根坐标轴
	enum KeType
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

	enum KeTextLayout
	{
		k_horz_top, // 标准横板，文字头部在上侧，底部在下侧
		k_horz_bottom, // 上下倒置的横版，文字头部在下侧，底部在上侧
		k_vert_left, // 竖版，文字头部在左侧，底部在右侧
		k_vert_right // 竖版，文字头部在右侧，底部在左侧
	};

	enum KeTickSide
	{
		k_inside, k_outside, k_bothside
	};

	struct KpTickContext : public KpPen
	{
		float length{ 5 };
		KeTickSide side{ k_outside };
		float yaw{ 0 }; // 绕坐标轴与tick的垂直线的旋转角度（弧度）
		float pitch{ 0 }; // 基于基准位置，绕坐标轴的旋转角度（弧度）

		KpPen& operator=(const KpPen& pen) {
			style = pen.style;
			width = pen.width;
			color = pen.color;
			return *this;
		}
	};

	struct KpTextContext
	{
		KpFont font;
		color4f color{ 0, 0, 0, 1 };
		KeTextLayout layout{ k_horz_top };
		bool billboard{ true }; // 是否以公告牌模式显示text，若true则text的hDir始终超向屏幕的右侧，vDir始终朝向屏幕的下侧
		float yaw{ 0 }; // 绕text-box平面的垂线（过中心点）的旋转角度（弧度）
		float pitch{ 0 }; // 绕hDir或vDir的旋转角度（弧度）
	};

	KcAxis(KeType type, int dim, bool main);

	KeType type() const { return type_; }
	void setType(KeType t) { type_ = t; }

	const point3& start() const { return start_; }
	void setStart(const point3& v) { start_ = v; }
	void setStart(float_t x, float_t y, float_t z) { start_ = point3(x, y, z); }

	const point3& end() const { return end_; }
	void setEnd(const point3& v) { end_ = v; }
	void setEnd(float_t x, float_t y, float_t z) { end_ = point3(x, y, z); }

	void setExtent(const point3& st, const point3& ed) {
		start_ = st, end_ = ed;
	}


	/// range 

	float_t lower() const { return lower_; }
	float_t& lower() { return lower_; }
	float_t upper() const { return upper_; }
	float_t& upper() { return upper_; }

	void setRange(float_t l, float_t u) {
		lower_ = l, upper_ = u;
	}

	float_t length() const { 
		return upper() - lower(); 
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

	const KpPen& baselineContext() const { return baselineCxt_; }
	KpPen& baselineContext() { return baselineCxt_; }

	std::shared_ptr<KvTicker> ticker() const;
	void setTicker(std::shared_ptr<KvTicker> tic);

	const KpTickContext& tickContext() const { return tickCxt_; }
	KpTickContext& tickContext() { return tickCxt_; }

	const KpTickContext& subtickContext() const { return subtickCxt_; }
	KpTickContext& subtickContext() { return subtickCxt_; }

	/// title properties

	const std::string& title() const { return title_; }
	std::string& title() { return title_; }

	float titlePadding() const { return titlePadding_; }
	float& titlePadding() { return titlePadding_; }

	KpTextContext titleContext() const { return titleCxt_; }
	KpTextContext& titleContext() { return titleCxt_; }

	/// label properties

	const std::vector<std::string>& labels() const { return labels_; }
	void setLabels(const std::vector<std::string>& ls) { labels_ = ls; }

	float labelPadding() const { return labelPadding_; }
	float& labelPadding() { return labelPadding_; }

	KpTextContext labelContext() const { return labelCxt_; }
	KpTextContext& labelContext() { return labelCxt_; }


	// NB：布局之后（即调用calcSize之后），该函数才能返回有效值
	aabb_t boundingBox() const override {
		return box_; 
	}

	void draw(KvPaint* paint) const override {
		return draw_(paint, false);
	}

	// 根据tick的数值，返回tick在坐标轴上的的3维坐标（世界坐标）
	point3 tickPos(double val) const;

	// 返回当前axis在屏幕坐标所占的尺寸（像素大小）
	KtMargins<float_t> calcMargins(KvPaint* paint) const;

	int dim() const { return dimReal_; }

	bool main() const { return main_; }

	bool inversed() const { return inv_; }
	void setInversed(bool inv) { inv_ = inv; }

	KeType typeReal() const; // 考虑swap，返回axis的真实方位布局

	// 内部使用
	void setSwapped_(int dimSwap) { dimSwapped_ = dimSwap; }

private:
	size_t calcSize_(void* cxt) const final;

	void draw_(KvPaint*, bool calcBox) const;
	void drawTicks_(KvPaint*, bool calcBox) const; // 绘制所有刻度
	void drawTick_(KvPaint*, const point3& anchor, double length, bool calcBox) const; // 绘制单条刻度线，兼容主刻度与副刻度
	void drawText_(KvPaint* paint, const std::string_view& label, const KpTextContext& cxt, const point3& anchor, bool calcBox) const;

	int labelAlignment_(KvPaint* paint, bool toggleTopBottom) const; // 根据label的orientation判定label的alignment
	bool tickAndLabelInSameSide_() const; // 判断tick与tick-label是否位于坐标轴的同侧

	// 计算tick的朝向
	vec3 calcTickOrient_(KvPaint*) const;

	point3 calcTitleAnchor_(KvPaint*) const;

	// 计算在3d空间绘制文本所需的3个参数：topLeft, hDir, vDir
	void calcTextPos_(KvPaint*, const std::string_view& label, const KpTextContext& cxt, 
		const point3& anchor, point3& topLeft, vec3& hDir, vec3& vDir) const;

	// 根据layout修正topLeft、hDir & vDir
	static void fixTextLayout_(KeTextLayout lay, const size_t& textBox, point3& topLeft, vec3& hDir, vec3& vDir);

private:
	KeType type_;
	std::string title_;
	std::vector<std::string> labels_; // tick labels
	float_t lower_, upper_; // range
	bool showBaseline_, showTick_, showSubtick_, showTitle_, showLabel_;

	KpPen baselineCxt_;
	KpTickContext tickCxt_, subtickCxt_;

	// label的上下文
	// 
	// 初始状态下，label-box位于刻度线与坐标轴构成的平面上
	// 
	// label-box的布局与姿态调整都相对anchor点进行
	// 当label位于坐标轴的左侧，其anchor点位于label-box的right-center
	// 当label位于坐标轴的右侧，其anchor点位于label-box的left-center
	// 当lebel位于坐标轴的下侧，其anchor点位于label-box的top-center
	// 当lebel位于坐标轴的上侧，其anchor点位于label-box的bottom-center
	// 在不考虑labelPadding_的情况下，若label与tick同侧，anchor点与刻度线的末端重合，否则与刻度点重合

	float labelPadding_{ 2 }; 
	KpTextContext labelCxt_;

	// title的上下文

	float titlePadding_{ 2 };
	KpTextContext titleCxt_;

	point3 start_, end_;

	std::shared_ptr<KvTicker> ticker_;

	int dimReal_; // 0表示x轴，1表示y轴，2表示z轴，-1表示不参与交换的数据轴（用来显示colorbar）
	int dimSwapped_; // -1表示另外两个维度的坐标轴交换
	bool main_{ true }; // 是否主坐标轴
	bool inv_{ false }; // 是否反转坐标轴


	// 以下为尺寸计算缓存的临时变量
	//
	mutable vec3 tickOrient_;
	mutable vec3 labelOrient_;
	std::vector<point2> labelSize_;
	mutable point2 titleSize_;
	mutable point3 titleAnchor_; // title文本框的top-left坐标
	vec3 hDir_, vDir_; // label文本的水平和垂直延展方向
	mutable aabb_t box_; // 预计算的aabb
};
