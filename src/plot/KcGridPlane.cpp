#include "KcGridPlane.h"
#include <assert.h>
#include "KcAxis.h"
#include "KvPaint.h"


KcGridPlane::KcGridPlane(axis_ptr h0, axis_ptr h1, axis_ptr v0, axis_ptr v1)
	: KvRenderable("GridPlane")
{
	horz_[0] = h0, horz_[1] = h1;
	vert_[0] = v0, vert_[1] = v1;

	minorVisible_ = false;

	majorLineCxt_.type = KpLineContext::k_line_solid;
	majorLineCxt_.width = 0.6;
	majorLineCxt_.color = color4f(0.3);

	minorLineCxt_.type = KpLineContext::k_line_dash;
	minorLineCxt_.width = 0.4;
	minorLineCxt_.color = color4f(0.3);
}


KcGridPlane::aabb_type KcGridPlane::boundingBox() const
{
	auto box1 = aabb_type{ point3::floor(horz_[0]->start(), horz_[1]->start()),
		point3::ceil(horz_[0]->end(), horz_[1]->end()) };
	auto box2 = aabb_type{ point3::floor(vert_[0]->start(), vert_[1]->start()),
		point3::ceil(vert_[0]->end(), vert_[1]->end()) };

	return box1.merge(box2);
}


void KcGridPlane::draw(KvPaint* paint) const
{
	assert(visible());
	paint->apply(majorLineCxt_);
	drawMajors_(paint, horz_[0], horz_[1]);
	drawMajors_(paint, vert_[0], vert_[1]);

	if (minorVisible()) {
		paint->apply(minorLineCxt_);
		drawMinors_(paint, horz_[0], horz_[1]);
		drawMinors_(paint, vert_[0], vert_[1]);
	}
}


void KcGridPlane::drawMajors_(KvPaint* paint, axis_ptr axis0, axis_ptr axis1)
{
	auto tic0 = axis0->ticker();
	auto tic1 = axis1->ticker();

	assert(tic0->ticks() == tic1->ticks()); // TODO: 

	auto vlen = axis1->start() - axis0->start();
	auto ticks = tic0->getTicks(axis0->lower(), axis0->upper()); // TODO: 隐藏该函数
	for (auto t : ticks) {
		auto pos = axis0->tickPos(t);
		paint->drawLine(pos, pos + vlen);
	}
}


void KcGridPlane::drawMinors_(KvPaint* paint, axis_ptr axis0, axis_ptr axis1)
{
	auto tic0 = axis0->ticker();
	auto tic1 = axis1->ticker();

	assert(tic0->subticks() == tic1->subticks()); // TODO:

	auto vlen = axis1->start() - axis0->start();
	auto ticks = tic0->getTicks(axis0->lower(), axis0->upper()); // TODO: 隐藏该函数
	auto subticks = tic0->getSubticks(ticks);
	for (auto t : subticks) {
		auto pos = axis0->tickPos(t);
		paint->drawLine(pos, pos + vlen);
	}
}
