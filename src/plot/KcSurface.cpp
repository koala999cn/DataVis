#include "KcSurface.h"
#include "KvPaint.h"
#include "KvSampled.h"
#include "KtGeometryImpl.h"
#include "KcVertexDeclaration.h"
#include "KuPrimitiveFactory.h"


void KcSurface::drawDiscreted_(KvPaint* paint, KvDiscreted* disc) const
{
	auto samp = dynamic_cast<KvSampled*>(disc);
	assert(samp && samp->dim() == 2);

	auto nx = samp->size(0);
	auto ny = samp->size(1);

	struct KpVtxBuffer_
	{
		point3f pos;
		point4f clr;
	};

	auto geom = std::make_shared<KtGeometryImpl<KpVtxBuffer_, unsigned>>(k_quads);
	auto vtx = geom->newVertex(nx* ny);
	auto vtxBuf = vtx;

	for(unsigned i = 0; i < nx; i++)
		for (unsigned j = 0; j < ny; j++) {
			auto pt = samp->point(i, j, 0);
			vtxBuf->pos = point3f(pt[0], pt[1], pt[2]);
			vtxBuf->clr = mapValueToColor_(pt[2]);
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

	paint->enableDepthTest(true);
	paint->drawGeom(decl, geom, true, showEdge);
	paint->enableDepthTest(false);
}
