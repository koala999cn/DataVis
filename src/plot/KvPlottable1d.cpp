#include "KvPlottable1d.h"
#include "KvDiscreted.h"

unsigned KvPlottable1d::objectCount() const
{
	if (empty())
		return 0;

	auto disc = discreted_();
	auto c = disc->channels() * objectsPerBatch_();
	return (disc->dim() > 1 && disc->isSampled()) ? c * discreted_()->size(0) : c;
}


void* KvPlottable1d::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	auto disc = discreted_();
	if (disc->dim() == 1)
		return draw1d_(paint, objIdx, disc.get());
	else if (disc->isSampled())
		return draw2d_(paint, objIdx, disc.get());
	else
		return draw3d_(paint, objIdx, disc.get());
}


void* KvPlottable1d::draw1d_(KvPaint* paint, unsigned objIdx, const KvDiscreted* disc) const
{
	auto ch = objIdx2ChsIdx_(objIdx);
	float_t z = defaultZ(ch);

	auto getter = [disc, ch, z](unsigned i) {
		auto pt = disc->pointAt(i, ch);
		pt.push_back(z);
		return pt;
	};

	return drawObjectImpl_(paint, getter, disc->size(), objIdx);
}


void* KvPlottable1d::draw2d_(KvPaint* paint, unsigned objIdx, const KvDiscreted* disc) const
{
	assert(disc->isSampled() && disc->dim() == 2);

	// 把objIdx分解到通道号和行号
	kIndex row = (objIdx / objectsPerBatch_()) % disc->size(0);
	kIndex ch = objIdx / objectsPerBatch_() / disc->size(0);

	auto getter = [disc, row, ch](unsigned i) {
		auto n = row * disc->size(1) * disc->channels() + i;
		return disc->pointAt(n, ch);
	};
	
	// NB: 与draw3d_不同之处在于，每个通道内draw2d_都逐行绘制（可绘制瀑布图）
	// 而draw3d_将每个通道数据作为一个整体进行绘制（没有行列概念）
	return drawObjectImpl_(paint, getter, disc->size(1), objIdx);
}


void* KvPlottable1d::draw3d_(KvPaint* paint, unsigned objIdx, const KvDiscreted* disc) const
{
	auto ch = objIdx2ChsIdx_(objIdx);
	auto getter = [disc, ch](unsigned i) {
		return disc->pointAt(i, ch);
	};

	return drawObjectImpl_(paint, getter, disc->size(), objIdx);
}


typename KvPaint::point_getter1 KvPlottable1d::toPoint3Getter_(GETTER g, unsigned channel) const
{
	if (this->usingDefaultZ_()) {
		auto z = defaultZ(channel);
		return [g, z](unsigned idx) {
			auto pt = g(idx);
			return point3(pt[0], pt[1], z);
		};
	}
	else {
		return [g](unsigned idx) {
			auto pt = g(idx);
			return point3(pt[0], pt[1], pt[2]);
		};
	}
}


unsigned KvPlottable1d::objIdx2ChsIdx_(unsigned objIdx) const
{
	assert(!empty());
	auto disc = discreted_();
	auto ch = objIdx / objectsPerBatch_();
	return (disc->isSampled() && disc->dim() == 2) ? ch / disc->size(0) : ch;
}
