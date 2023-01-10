#pragma once
#include "KvRenderable.h"
#include <memory>
#include "KpContext.h"
#include "layout/KcLayoutOverlay.h"

class KcAxis;

class KcCoordPlane : public KvRenderable, public KcLayoutOverlay
{
	using KvRenderable::float_t;
	using axis_ptr = std::shared_ptr<KcAxis>;
	using KvRenderable::aabb_t;
	using point3 = KtPoint<float_t, 3>;

public:

	// 3d坐标系中的6个坐标平面
	enum KePlaneType
	{
		// x-y plane
		k_back,
		k_front,

		// y-z plane
		k_left,
		k_right,

		// x-z plane
		k_ceil,
		k_floor,
	};

	enum KeGridMode
	{
		k_grid_horz = 0x01,
		k_grid_vert = 0x02,
		k_grid_both = k_grid_horz | k_grid_vert
	};


	KcCoordPlane(KePlaneType type, axis_ptr h0, axis_ptr h1, axis_ptr v0, axis_ptr v1);

	KePlaneType type() const { return type_; }

	bool majorVisible() const { return majorVisible_; }
	bool& majorVisible() { return majorVisible_; }

	bool minorVisible() const { return minorVisible_; }
	bool& minorVisible() { return minorVisible_; }

	int majorMode() const { return majorMode_; }
	int& majorMode() { return majorMode_; }

	int minorMode() const { return minorMode_; }
	int& minorMode() { return minorMode_; }

	const KpPen& majorLine() const { return majorLineCxt_; }
	KpPen& majorLine() { return majorLineCxt_; }

	const KpPen& minorLine() const { return minorLineCxt_; }
	KpPen& minorLine() { return minorLineCxt_; }

	const KpBrush& background() const { return bkgnd_; }
	KpBrush& background() { return bkgnd_; }

	aabb_t boundingBox() const override;

	void draw(KvPaint*) const override;

private:

	// 绘制从axis0轴到axis1轴的主grid
	static void drawMajors_(KvPaint*, axis_ptr axis0, axis_ptr axis1);

	// 绘制从axis0轴到axis1轴的副grid
	static void drawMinors_(KvPaint*, axis_ptr axis0, axis_ptr axis1);

private:
	KePlaneType type_;
	axis_ptr horz_[2], vert_[2]; // 构成grid平面的4根坐标轴，水平2根、垂直2根
	bool majorVisible_{ false }, minorVisible_{ false };
	KpPen majorLineCxt_, minorLineCxt_;
	int majorMode_{ k_grid_both }, minorMode_{ k_grid_both };
	KpBrush bkgnd_;
};
