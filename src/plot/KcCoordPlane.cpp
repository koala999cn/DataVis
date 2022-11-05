#include "KcCoordPlane.h"
#include <assert.h>
#include "KcAxis.h"
#include "KvPaint.h"


KcCoordPlane::KcCoordPlane(KePlaneType type, axis_ptr h0, axis_ptr h1, axis_ptr v0, axis_ptr v1)
	: KvRenderable("GridPlane")
	, type_(type)
{
	horz_[0] = h0, horz_[1] = h1;
	vert_[0] = v0, vert_[1] = v1;

	minorVisible_ = false;

	majorLineCxt_.style = KpPen::k_solid;
	majorLineCxt_.width = 0.7;
	majorLineCxt_.color = color4f(0.5);

	minorLineCxt_.style = KpPen::k_dash;
	minorLineCxt_.width = 0.5;
	minorLineCxt_.color = color4f(0.5);
}


KcCoordPlane::aabb_type KcCoordPlane::boundingBox() const
{
	auto box1 = aabb_type{ point3::floor(horz_[0]->start(), horz_[1]->start()),
		point3::ceil(horz_[0]->end(), horz_[1]->end()) };
	auto box2 = aabb_type{ point3::floor(vert_[0]->start(), vert_[1]->start()),
		point3::ceil(vert_[0]->end(), vert_[1]->end()) };

	return box1.merge(box2);
}


void KcCoordPlane::draw(KvPaint* paint) const
{
	assert(visible());

	if (bkgnd_.style != KpBrush::k_none) {
		paint->apply(background());
		paint->fillQuad(horz_[0]->start(), horz_[0]->end(), horz_[1]->end(), horz_[1]->start());
	}

	bool showMajor = majorVisible() && majorLineCxt_.style != KpPen::k_none;
	bool showMinor = minorVisible() && minorLineCxt_.style != KpPen::k_none;

	axis_ptr axes[] = {
		horz_[0], horz_[1], vert_[0], vert_[1]
	};

	for(unsigned i = 0; i < std::size(axes); i++)
		axes[i]->scaler()->generate(axes[i]->lower(), axes[i]->upper(), showMajor, showMinor);

	if (showMajor) {
		paint->apply(majorLineCxt_);
		drawMajors_(paint, horz_[0], horz_[1]);
		drawMajors_(paint, vert_[0], vert_[1]);
	}

	if (showMinor) {
		paint->apply(minorLineCxt_);
		drawMinors_(paint, horz_[0], horz_[1]);
		drawMinors_(paint, vert_[0], vert_[1]);
	}
}


void KcCoordPlane::drawMajors_(KvPaint* paint, axis_ptr axis0, axis_ptr axis1)
{
	auto scale0 = axis0->scaler();
	auto scale1 = axis1->scaler();

	assert(scale0->tickCount() == scale1->tickCount()); // TODO: 

	auto vlen = axis1->start() - axis0->start();
	auto& ticks = scale0->ticks(); // TODO: 假定ticks已生成
	for (auto t : ticks) {
		auto pos = axis0->tickPos(t);
		paint->drawLine(pos, pos + vlen);
	}
}


void KcCoordPlane::drawMinors_(KvPaint* paint, axis_ptr axis0, axis_ptr axis1)
{
	auto scale0 = axis0->scaler();
	auto scale1 = axis1->scaler();

	assert(scale0->subtickCount() == scale1->subtickCount()); // TODO:

	auto vlen = axis1->start() - axis0->start();
	auto& subticks = scale0->subticks();
	for (auto t : subticks) {
		auto pos = axis0->tickPos(t);
		paint->drawLine(pos, pos + vlen);
	}
}
