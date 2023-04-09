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


KcCoordPlane::aabb_t KcCoordPlane::boundingBox() const
{
	auto box1 = aabb_t{ point3::floor(horz_[0]->start(), horz_[1]->start()),
		point3::ceil(horz_[0]->end(), horz_[1]->end()) };
	auto box2 = aabb_t{ point3::floor(vert_[0]->start(), vert_[1]->start()),
		point3::ceil(vert_[0]->end(), vert_[1]->end()) };

	return box1.merge(box2);
}


void KcCoordPlane::draw(KvPaint* paint) const
{
	assert(visible());

	if (bkgnd_.visible()) {
		paint->apply(bkgnd_);
		point3 pts[4] = { horz_[0]->start(), horz_[0]->end(), horz_[1]->end(), horz_[1]->start() };
		paint->fillQuad(pts);
	}

	bool showMajor = majorVisible() && majorLineCxt_.visible();
	bool showMinor = minorVisible() && minorLineCxt_.visible();

	axis_ptr axes[] = {
		horz_[0], horz_[1], vert_[0], vert_[1]
	};

	for(unsigned i = 0; i < std::size(axes); i++)
		axes[i]->ticker()->update(axes[i]->lower(), axes[i]->upper(), !showMinor);

	if (showMajor) {
		paint->apply(majorLineCxt_);
		if (majorMode_ & k_grid_vert)
		    drawMajors_(paint, horz_[0], horz_[1]);
		if (majorMode_ & k_grid_horz)
		    drawMajors_(paint, vert_[0], vert_[1]);
	}

	if (showMinor) {
		paint->apply(minorLineCxt_);
		if (minorMode_ & k_grid_vert)
		    drawMinors_(paint, horz_[0], horz_[1]);
		if (minorMode_ & k_grid_horz)
		    drawMinors_(paint, vert_[0], vert_[1]);
	}
}


void KcCoordPlane::drawMajors_(KvPaint* paint, axis_ptr axis0, axis_ptr axis1)
{
	auto t0 = axis0->ticker();
	auto t1 = axis1->ticker();

	//assert(t0->tickCount() == t1->tickCount()); // TODO: 

	auto vlen = axis1->start() - axis0->start();
	for (unsigned i = 0; i < t0->ticksTotal(); i++) {  // TODO: 假定ticks已生成
		auto pos = axis0->tickPos(t0->tick(i));
		paint->drawLine(pos, pos + vlen);
	}
}


void KcCoordPlane::drawMinors_(KvPaint* paint, axis_ptr axis0, axis_ptr axis1)
{
	auto t0 = axis0->ticker();
	auto t1 = axis1->ticker();

	//assert(t0->subtickCount() == t1->subtickCount()); // TODO:

	auto vlen = axis1->start() - axis0->start();
	for (unsigned i = 0; i < t0->subticksTotal(); i++) {
		auto pos = axis0->tickPos(t0->subtick(i));
		paint->drawLine(pos, pos + vlen);
	}
}
