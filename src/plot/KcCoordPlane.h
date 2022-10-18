#pragma once
#include "KvRenderable.h"
#include <memory>
#include "KpContext.h"

class KcAxis;

class KcCoordPlane : public KvRenderable
{
	using axis_ptr = std::shared_ptr<KcAxis>;
	using KvRenderable::aabb_type;
	using point3 = KtPoint<float_t, 3>;

public:
	KcCoordPlane(axis_ptr h0, axis_ptr h1, axis_ptr v0, axis_ptr v1);

	bool minorVisible() const { return minorVisible_; }
	bool& minorVisible() { return minorVisible_; }

	const KpPen& majorLine() const { return majorLineCxt_; }
	KpPen& majorLine() { return majorLineCxt_; }

	const KpPen& minorLine() const { return minorLineCxt_; }
	KpPen& minorLine() { return minorLineCxt_; }

	const KpBrush& background() const { return bkgnd_; }
	KpBrush& background() { return bkgnd_; }

	aabb_type boundingBox() const override;

	void draw(KvPaint*) const override;

private:

	// 绘制从axis0轴到axis1轴的主grid
	static void drawMajors_(KvPaint*, axis_ptr axis0, axis_ptr axis1);

	// 绘制从axis0轴到axis1轴的副grid
	static void drawMinors_(KvPaint*, axis_ptr axis0, axis_ptr axis1);

private:
	axis_ptr horz_[2], vert_[2]; // 构成grid平面的4根坐标轴，水平2根、垂直2根
	bool minorVisible_;
	KpPen majorLineCxt_, minorLineCxt_;
	KpBrush bkgnd_;
};
