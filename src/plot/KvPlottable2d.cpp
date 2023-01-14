#include "KvPlottable2d.h"
#include "KvSampled.h"
#include "KvPaint.h"
#include "KtGeometryImpl.h"
#include "KcVertexDeclaration.h"
#include "KuPrimitiveFactory.h"


KvPlottable2d::aabb_t KvPlottable2d::boundingBox() const
{
	auto box = super_::boundingBox();
	if (!empty() && forceDefaultZ_) {
		box.lower().z() = defaultZ();
		box.upper().z() = defaultZ() + stepZ() * data()->channels();
		if (stepZ() < 0)
			std::swap(box.lower().z(), box.upper().z());
	}
	return box;
}


const color4f& KvPlottable2d::minorColor() const
{
	return borderPen_.color;
}


void KvPlottable2d::setMinorColor(const color4f& minor)
{
	borderPen_.color = minor;
}


void KvPlottable2d::drawDiscreted_(KvPaint* paint, KvDiscreted* disc) const
{
	auto samp = dynamic_cast<KvSampled*>(disc);
	assert(samp && samp->dim() >= 2);

	unsigned ch(0);
	auto getter = [&samp, &ch](unsigned ix, unsigned iy) -> KvPaint::point3 {
		auto pt = samp->point(ix, iy, ch);
		return { pt[0], pt[1], pt[2] };
	};

	for (; ch < samp->channels(); ch++)
		drawImpl_(paint, getter, samp->size(0), samp->size(1), ch);
}


void KvPlottable2d::drawImpl_(KvPaint* paint, point_getter2 getter, unsigned nx, unsigned ny, unsigned ch) const
{
	struct KpVtxBuffer_
	{
		point3f pos;
		point4f clr;
	};

	auto geom = std::make_shared<KtGeometryImpl<KpVtxBuffer_, unsigned>>(k_quads);
	auto vtx = geom->newVertex(nx * ny);
	auto vtxBuf = vtx;

	for (unsigned i = 0; i < nx; i++)
		for (unsigned j = 0; j < ny; j++) {
			auto pt = getter(i, j);
			assert(pt.size() > 2);
			
			vtxBuf->pos = point3f(pt[0], pt[1], forceDefaultZ_ ? defaultZ(ch) : pt[2]);
			vtxBuf->clr = mapValueToColor_(pt.back(), ch); // TODO: 可以选择色彩插值的维度
			++vtxBuf;
		}

	auto idxCount = KuPrimitiveFactory::indexGrid<unsigned>(nx, ny, nullptr);
	auto idxBuf = geom->newIndex(idxCount);
	KuPrimitiveFactory::indexGrid<unsigned>(nx, ny, idxBuf);

	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
	decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_diffuse);

	bool showEdge = showBorder() && borderPen().visible();
	if (showEdge)
		paint->apply(borderPen());

	//paint->enableDepthTest(true); // TODO: 可配置
	paint->drawGeom(decl, geom, true, showEdge);
	//paint->enableDepthTest(false);
}
