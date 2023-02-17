#include "KvPlottable2d.h"
#include "KvSampled.h"
#include "KvPaint.h"
#include "KtGeometryImpl.h"
#include "KuPrimitiveFactory.h"


const color4f& KvPlottable2d::minorColor() const
{
	return borderPen_.color;
}


void KvPlottable2d::setMinorColor_(const color4f& minor)
{
	borderPen_.color = minor;
}


unsigned KvPlottable2d::objectCount() const
{
	return data()->channels();
}


bool KvPlottable2d::objectVisible_(unsigned objIdx) const
{
	return filled_ || (showBorder() && borderPen().visible());
}


void KvPlottable2d::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	bool edged = showBorder() && borderPen().visible();
	paint->setFilled(filled_);
	paint->setEdged(edged);

	if (edged)
		paint->apply(borderPen());
}


void* KvPlottable2d::drawObject_(KvPaint* paint, unsigned objIdx, const KvDiscreted* disc) const
{
	auto samp = dynamic_cast<const KvSampled*>(disc);
	assert(samp && samp->dim() >= 2);

	auto getter = [&samp, objIdx](unsigned ix, unsigned iy) {
		return samp->point(ix, iy, objIdx);
	};

	return drawImpl_(paint, getter, samp->size(0), samp->size(1), objIdx);
}


void* KvPlottable2d::drawImpl_(KvPaint* paint, GETTER getter, unsigned nx, unsigned ny, unsigned ch) const
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
			auto pt = getter(i, j); // getter返回原值，未处理z值的强制替换（若需要）
			assert(pt.size() > 2);
			vtxBuf->pos = toPoint_(pt.data(), ch);
			vtxBuf->clr = mapValueToColor_(pt.data(), ch); // 传入原值
			++vtxBuf;
		}

	auto idxCount = KuPrimitiveFactory::indexGrid<unsigned>(nx, ny, nullptr);
	auto idxBuf = geom->newIndex(idxCount);
	KuPrimitiveFactory::indexGrid<unsigned>(nx, ny, idxBuf);

	return paint->drawGeomColor(geom);
}
