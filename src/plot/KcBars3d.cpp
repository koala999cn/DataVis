#include "KcBars3d.h"
#include "KuPrimitiveFactory.h"
#include "KtGeometryImpl.h"
#include "KcVertexDeclaration.h"
#include "KvData.h"


KcBars3d::aabb_t KcBars3d::boundingBox() const
{
	auto aabb = super_::boundingBox();

	if (aabb.depth() == 0)
		aabb.upper().z() = aabb.lower().z() + 1;
	return aabb;
}


void KcBars3d::drawImpl_(KvPaint* paint, point_getter getter, unsigned count, unsigned) const
{
	auto dim = data()->dim();
	auto xw = barWidth_(0);
	auto yw = dim == 1 ? boundingBox().depth() / count : barWidth_(1);

	bool drawFill = fill_.style != KpBrush::k_none && majorColor(0).a() != 0;
	bool drawBorder = border_.style != KpPen::k_none && minorColor().a() != 0 && minorColor() != majorColor(0);

	struct KpVtxBuffer_
	{
		point3f pos;
		point3f normal;
	};

	auto geom = std::make_shared<KtGeometryImpl<KpVtxBuffer_, unsigned>>(k_triangles);
	auto vtxPerBox = 8;
	auto idxPerBox = 36;
	geom->reserve(vtxPerBox * count, idxPerBox * count);

	for (unsigned i = 0; i < count; i++) {
		auto pt0 = getter(i);
		point3 pt1;

		if (dim == 1) {
			pt0.x() += xw * 0.5; pt0.z() += yw * 0.5;
			pt1 = { pt0.x() - xw, baseLine_, pt0.z() - yw };
		}
		else {
			pt0.x() += xw * 0.5; pt0.y() += yw * 0.5;
			pt1 = { pt0.x() - xw, pt0.y() - yw, baseLine_ };
		}

		unsigned idxBase = geom->vertexCount();
		char* vtxBuf = (char*)geom->newVertex(vtxPerBox);
		KuPrimitiveFactory::makeBox<KuPrimitiveFactory::k_position, float>(pt1, pt0, vtxBuf, sizeof(KpVtxBuffer_));
		KuPrimitiveFactory::makeBox<KuPrimitiveFactory::k_normal, float>(pt1, pt0, vtxBuf + sizeof(point3f), sizeof(KpVtxBuffer_));

		auto* idxBuf = geom->newIndex(idxPerBox);
		KuPrimitiveFactory::makeBox<KuPrimitiveFactory::k_mesh_index, unsigned>(pt1, pt0, idxBuf);
		for (unsigned i = 0; i < idxPerBox; i++)
			idxBuf[i] += idxBase;
	}

	if (drawFill) {
		paint->apply(fill_);
		auto decl = std::make_shared<KcVertexDeclaration>();
		decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
		decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_normal);
		paint->drawGeom(decl, geom);
	}

	// TODO:
	//if (drawBorder) {
	//	paint->apply(border_);
	//	paint->drawRect(pt0, pt1);
	//}
}