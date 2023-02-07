#include "KcBars3d.h"
#include "KuPrimitiveFactory.h"
#include "KtGeometryImpl.h"
#include "KcVertexDeclaration.h"
#include "KvData.h"


KcBars3d::aabb_t KcBars3d::calcBoundingBox_() const
{
	auto aabb = super_::calcBoundingBox_();

	if (aabb.depth() == 0)
		aabb.upper().z() = aabb.lower().z() + 1;
	return aabb;
}

#if 0
void KcBars3d::drawImpl_(KvPaint* paint, point_getter1 getter, unsigned count, unsigned) const
{
	auto dim = data()->dim();
	auto xw = barWidth_(0);
	auto yw = dim == 1 ? boundingBox().depth() / count : barWidth_(1);

	bool drawFill = fill_.visible();
	bool drawBorder = border_.visible(); // && minorColor() != majorColor(0);
	if (drawBorder)
		paint->apply(border_);

	struct KpVtxBuffer_
	{
		point3f pos;
		color4f color;
		//point3f normal; // TODO: lighting
	};

	auto geom = std::make_shared<KtGeometryImpl<KpVtxBuffer_, unsigned>>(k_quads);
	auto vtxCount = KuPrimitiveFactory::makeBox<float>(point3f(0), point3f(0), nullptr);
	auto idxCount = KuPrimitiveFactory::indexBox<unsigned>(nullptr);
	assert(idxCount == 6 * 4); // 断言索引为quad类型
	geom->reserve(vtxCount* count, idxCount* count);

	for (unsigned i = 0; i < count; i++) {
		auto pt0 = getter(i);
		decltype(pt0) pt1;

		if (dim == 1) {
			pt0.x() += xw * 0.5; pt0.z() += yw * 0.5;
			pt1 = { pt0.x() - xw, baseLine_, pt0.z() - yw };
		}
		else {
			pt0.x() += xw * 0.5; pt0.y() += yw * 0.5;
			pt1 = { pt0.x() - xw, pt0.y() - yw, baseLine_ };
		}

		unsigned idxBase = geom->vertexCount();
		auto vtxBuf = geom->newVertex(vtxCount);
		KuPrimitiveFactory::makeBox<float>(point3f(pt1), point3f(pt0), vtxBuf, sizeof(KpVtxBuffer_));
		for (unsigned i = 0; i < vtxCount; i++)
			vtxBuf[i].color = fill_.color;

		auto idxBuf = geom->newIndex(idxCount);
		KuPrimitiveFactory::indexBox<unsigned>(idxBuf);
		for (unsigned i = 0; i < idxCount; i++)
			idxBuf[i] += idxBase;
	}

	paint->enableDepthTest(true);
	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
	decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_diffuse);
	paint->drawGeom(decl, geom, drawFill, drawBorder);
	paint->enableDepthTest(false);
}
#endif