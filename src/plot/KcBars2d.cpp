#include "KcBars2d.h"
#include "KvPaint.h"
#include "KvDiscreted.h"
#include "KvContinued.h"
#include "KtGeometryImpl.h"
#include "KcVertexDeclaration.h"


KcBars2d::KcBars2d(const std::string_view& name)
	: super_(name)
{
	border_.color = color4f(0, 0, 0, 0); // default no border 
}


void KcBars2d::drawDiscreted_(KvPaint* paint, KvDiscreted* disc) const
{
	auto barWidth = barWidth_();

	struct KpVtxBuffer_
	{
		point3f pos;
		point4f clr;
	};

	auto geom = std::make_shared<KtGeometryImpl<KpVtxBuffer_, unsigned>>(k_quads);
	auto vtx = geom->newVertex(disc->size() * disc->channels() * 4);
	auto vtxBuf = vtx;

	for (unsigned i = 0; i < disc->size(); i++) {

		auto bottom = baseLine_;

		for (unsigned j = 0; j < disc->channels(); j++) {
			auto pt = disc->pointAt(i, j);
			if (pt.size() < 3)
				pt.push_back(defaultZ(j));

			auto top = bottom + pt[1];
			auto left = pt[0] - barWidth / 2;
			auto right = pt[0] + barWidth / 2;

			// 第一个顶点取right-top，这样可保证最后一个顶点为left-top（quad各顶点按顺时针排列）
			// 如此确保在flat模式下显示left-top顶点的颜色（保证按y轴插值时的正确性）
			vtxBuf[0].pos = point3f(right, top, pt[2]);
			pt[0] = right, pt[1] = top;
			vtxBuf[0].clr = mapValueToColor_(pt.data(), j);

			vtxBuf[1].pos = point3f(right, bottom, pt[2]);
			pt[1] = bottom;
			vtxBuf[1].clr = mapValueToColor_(pt.data(), j);

			vtxBuf[2].pos = point3f(left, bottom, pt[2]);
			pt[0] = left;
			vtxBuf[2].clr = mapValueToColor_(pt.data(), j);

			vtxBuf[3].pos = point3f(left, top, pt[2]);
			pt[1] = top;
			vtxBuf[3].clr = mapValueToColor_(pt.data(), j);

			vtxBuf += 4;

			bottom = top;
		}
	}

	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
	decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_diffuse);

	bool showEdge = showBorder() && borderPen().visible();
	if (showEdge)
		paint->apply(borderPen());

	paint->drawGeom(decl, geom, true, showEdge);
}


KcBars2d::float_t KcBars2d::barWidth_(unsigned dim) const
{
	assert(barWidthRatio_ > 0);

	auto disc = discreted_();
	assert(disc && disc->size(dim) != 0);

	return disc->step(dim) != 0 ?
		disc->step(dim) * barWidthRatio_ :
		disc->range(dim).length() / disc->size(dim) * barWidthRatio_;
}


KcBars2d::aabb_t KcBars2d::boundingBox() const
{
	auto aabb = super_::boundingBox();

	if (!empty()) {
		auto w = barWidth_();
		aabb.inflate(w, 0);

		if (stacked_) {
			auto r = stackedRange_();
			auto pt = aabb.lower();
			pt.y() = baseLine_ + r.first; aabb.merge(pt);
			pt.y() = baseLine_ + r.second; aabb.merge(pt);
		}
	}

	return aabb;
}


const color4f& KcBars2d::minorColor() const
{
	return border_.color;
}


void KcBars2d::setMinorColor(const color4f& minor)
{
	border_.color = minor;
}


std::pair<KcBars2d::float_t, KcBars2d::float_t> KcBars2d::stackedRange_() const
{
	if (data()->channels() == 1) {
		auto r = data()->range(1);
		return { r.low(), r.high() };
	}

	auto in(std::numeric_limits<float_t>::max());
	auto ax(std::numeric_limits<float_t>::lowest());

	auto disc = discreted_();
	assert(disc);
	for (unsigned i = 0; i < disc->size(); i++) {
		float_t v(0);
		for (unsigned ch = 0; ch < disc->channels(); ch++)
			v += disc->pointAt(i, ch)[1];
		if (v < in) in = v;
		if (v > ax) ax = v;
	}

	return { in, ax };
}

