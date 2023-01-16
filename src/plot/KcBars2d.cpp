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
	assert(disc->dim() == 1); // TODO: 暂时只支持1d数据

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
		for (unsigned j = 0; j < disc->channels(); j++) {
			auto pt = disc->pointAt(i, j);
			if (pt.size() < 3)
				pt.push_back(defaultZ(j));

			auto clr = mapValueToColor_(pt.data(), j);

			// 第一个顶点取top-right，这样可保证最后一个顶点为top-left（quad各顶点按顺时针排列）
			// 如此确保在flat模式下显示top-left顶点的颜色
			vtxBuf[0].pos = point3f(pt[0] + barWidth / 2, pt[1], pt[2]);
			vtxBuf[0].clr = clr;

			vtxBuf[1].pos = point3f(pt[0] + barWidth / 2, baseLine_, pt[2]);
			vtxBuf[1].clr = clr;

			vtxBuf[2].pos = point3f(pt[0] - barWidth / 2, baseLine_, pt[2]);
			vtxBuf[2].clr = clr;

			vtxBuf[3].pos = point3f(pt[0] - barWidth / 2, pt[1], pt[2]);
			vtxBuf[3].clr = clr;

			vtxBuf += 4;;
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

	if (data()->isDiscreted()) {
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(data());
		assert(disc->size(dim) != 0);

		return disc->step(dim) != 0 ?
			disc->step(dim) * barWidthRatio_ :
			disc->range(dim).length() / disc->size(dim) * barWidthRatio_;
	}
	else {
		assert(sampCount(dim) != 0);
		auto cont = std::dynamic_pointer_cast<KvContinued>(data());
		return cont->range(dim).length() / sampCount(dim) * barWidthRatio_;
	}
}


KcBars2d::aabb_t KcBars2d::boundingBox() const
{
	auto aabb = super_::boundingBox();
	if (aabb.lower().y() > baseLine_)
		aabb.lower().y() = baseLine_;
	if (aabb.upper().y() < baseLine_)
		aabb.upper().y() = baseLine_;

	if (!empty()) {
		auto w = barWidth_();
		aabb.lower().x() -= w;
		aabb.upper().x() += w;
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
