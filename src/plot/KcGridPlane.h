#pragma once
#include "KvRenderable.h"
#include <memory>
#include "KtColor.h"

class KcAxis;

class KcGridPlane : public KvRenderable
{
	using axis_ptr = std::shared_ptr<KcAxis>;
	using aabb_type = typename KvRenderable::aabb_tyle;

public:
	KcGridPlane(axis_ptr h0, axis_ptr h1, axis_ptr v0, axis_ptr v1);

	bool minorVisible() const { return minorVisible_; }
	void setMinorVisible(bool b) { minorVisible_ = b; }

/*	color4f majorColor() const {
		return majorLine_.color;
	}
	void setMajorColor(const color4f& color) {
		majorLine_.color = color;
	}

	int majorStyle() const {
		return majorLine_.style;
	}
	void setMajorStyle(int style) {
		majorLine_.style = style;
	}

	double majorWidth() const {
		return majorLine_.width;
	}
	void setMajorWidth(double width) {
		majorLine_.width = width;
	}

	color4f minorColor() const {
		return minorLine_.color;
	}
	void setMinorColor(const color4f& color) {
		minorLine_.color = color;
	}

	int minorStyle() const {
		return minorLine_.style;
	}
	void setMinorStyle(int style) {
		minorLine_.style = style;
	}

	double minorWidth() const {
		return minorLine_.width;
	}
	void setMinorWidth(double width) {
		minorLine_.width = width;
	}*/

	aabb_type boundingBox() const override;

	void draw(KvPaint*) const override;

private:

	// ���ƴ�axis0�ᵽaxis1�����grid
	static void drawMajors_(KvPaint*, axis_ptr axis0, axis_ptr axis1);

	// ���ƴ�axis0�ᵽaxis1��ĸ�grid
	static void drawMinors_(KvPaint*, axis_ptr axis0, axis_ptr axis1);

private:
	axis_ptr horz_[2], vert_[2]; // ����gridƽ���4�������ᣬˮƽ2������ֱ2��
	bool minorVisible_;
	//KpLine majorLine_, minorLine_;
};
