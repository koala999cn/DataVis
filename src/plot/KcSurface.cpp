#include "KcSurface.h"
#include "KvPaint.h"
#include "KtGeometryImpl.h"
#include "KuPrimitiveFactory.h"
#include "KvDiscreted.h"


void* KcSurface::drawObject_(KvPaint* paint, unsigned ch) const
{
	bool solid = coloringMode() == k_one_color_solid;
	return solid ? drawSolid_(paint, ch) : drawColor_(paint, ch);
}


void* KcSurface::drawSolid_(KvPaint* paint, unsigned ch) const
{
	auto nx = sizePerLine_();
	auto nz = linesPerGrid_();
	auto grids = gridsPerChannel_();

	auto geom = std::make_shared<KtGeometryImpl<point3f, std::uint32_t>>(k_quads);
	auto idxPerGrid = KuPrimitiveFactory::indexGrid<std::uint32_t>(nx, nz, nullptr);
	geom->reserve(nx * nz * gridsTotal_(), idxPerGrid * gridsTotal_());
	std::uint32_t idxBase(0);

	for (unsigned i = 0; i < grids; i++) {
		for (unsigned j = 0; j < nz; j++) {
			auto line = gridLineAt_(ch, i, j);
			assert(line.size == nx);

			auto vtx = geom->newVertex(nx);
			for (unsigned k = 0; k < nx; k++) {
				auto pt = line.getter(k);
				*vtx = toPoint_(pt.data(), ch);
				++vtx;
			}
		}

		auto idx = geom->newIndex(idxPerGrid);
		KuPrimitiveFactory::indexGrid<std::uint32_t>(nx, nz, idx, 0, idxBase);
		idxBase += nx * nz;
	}

	return paint->drawGeomSolid(geom);
}


void* KcSurface::drawColor_(KvPaint* paint, unsigned ch) const
{
	struct KpVtxBuffer_
	{
		point3f pos;
		point4f clr;
	};

	auto nx = sizePerLine_();
	auto nz = linesPerGrid_();
	auto grids = gridsPerChannel_();

	auto geom = std::make_shared<KtGeometryImpl<KpVtxBuffer_, std::uint32_t>>(k_quads);
	auto idxPerGrid = KuPrimitiveFactory::indexGrid<std::uint32_t>(nx, nz, nullptr);
	geom->reserve(nx * nz * gridsTotal_(), idxPerGrid * gridsTotal_());
	std::uint32_t idxBase(0);

	for (unsigned i = 0; i < grids; i++) {
		for (unsigned j = 0; j < nz; j++) {
			auto line = gridLineAt_(ch, i, j);
			assert(line.size == nx);

			auto vtx = geom->newVertex(nx);
			for (unsigned k = 0; k < nx; k++) {
				auto pt = line.getter(k);
				vtx->pos = toPoint_(pt.data(), ch);
				vtx->clr = mapValueToColor_(pt.data(), ch);
				++vtx;
			}
		}

		auto idx = geom->newIndex(idxPerGrid);
		KuPrimitiveFactory::indexGrid<std::uint32_t>(nx, nz, idx, 0, idxBase);
		idxBase += nx * nz;
	}

	return paint->drawGeomColor(geom);
}
