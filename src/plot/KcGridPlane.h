#pragma once
#include "KvRenderable.h"
#include <memory>
#include "KtColor.h"
#include "KpContext.h"

class KcAxis;

class KcGridPlane : public KvRenderable
{
	using axis_ptr = std::shared_ptr<KcAxis>;
	using KvRenderable::aabb_type;
	using point3 = KtPoint<float_t, 3>;

public:
	KcGridPlane(axis_ptr h0, axis_ptr h1, axis_ptr v0, axis_ptr v1);

	bool minorVisible() const { return minorVisible_; }
	bool& minorVisible() { return minorVisible_; }

	const KpPen& majorLine() const { return majorLineCxt_; }
	KpPen& majorLine() { return majorLineCxt_; }

	const KpPen& minorLine() const { return minorLineCxt_; }
	KpPen& minorLine() { return minorLineCxt_; }

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
	KpPen majorLineCxt_, minorLineCxt_;
};
