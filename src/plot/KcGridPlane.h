#pragma once
#include "KvRenderable.h"
#include <memory>
#include "KglPaint.h"

class KcAxis;

class KcGridPlane : public KvRenderable
{
	using axis_ptr = std::shared_ptr<KcAxis>;
	using vec4 = KtVector4<double>;

public:
	KcGridPlane(axis_ptr h0, axis_ptr h1, axis_ptr v0, axis_ptr v1);

	void draw(KglPaint*) const override;

	bool minorVisible() const { return minorVisible_; }
	void setMinorVisible(bool b) { minorVisible_ = b; }

	vec4 majorColor() const {
		return majorLine_.color;
	}
	void setMajorColor(const vec4& color) {
		majorLine_.color = color;
	}

	int majorStyle() const {
		return majorLine_.style;
	}
	void setMajorStyle(int style) {
		majorLine_.style = style;
	}

	int majorWidth() const {
		return majorLine_.width;
	}
	void setMajorWidth(double width) {
		majorLine_.width = width;
	}

	vec4 minorColor() const {
		return minorLine_.color;
	}
	void setMinorColor(const vec4& color) {
		minorLine_.color = color;
	}

	int minorStyle() const {
		return minorLine_.style;
	}
	void setMinorStyle(int style) {
		minorLine_.style = style;
	}

	int minorWidth() const {
		return minorLine_.width;
	}
	void setMinorWidth(double width) {
		minorLine_.width = width;
	}

private:

	// 绘制从axis0轴到axis1轴的主grid
	static void drawMajors_(KglPaint*, axis_ptr axis0, axis_ptr axis1);

	// 绘制从axis0轴到axis1轴的副grid
	static void drawMinors_(KglPaint*, axis_ptr axis0, axis_ptr axis1);

private:
	axis_ptr horz_[2], vert_[2]; // 构成grid平面的4根坐标轴，水平2根、垂直2根
	bool minorVisible_;
	KpLine majorLine_, minorLine_;
};
