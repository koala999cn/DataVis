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


void KvPlottable2d::drawDiscreted_(KvPaint* paint, const KvDiscreted* disc) const
{
	auto samp = dynamic_cast<const KvSampled*>(disc);
	assert(samp && samp->dim() >= 2);

	unsigned ch(0);
	auto getter = [&samp, &ch](unsigned ix, unsigned iy) {
		return samp->point(ix, iy, ch);
	};

	for (; ch < samp->channels(); ch++)
		drawImpl_(paint, getter, samp->size(0), samp->size(1), ch);
}


void KvPlottable2d::drawImpl_(KvPaint* paint, GETTER getter, unsigned nx, unsigned ny, unsigned ch) const
{
	bool showEdge = showBorder() && borderPen().visible();
	if (showEdge)
		paint->apply(borderPen());

	if (renderObj_.size() < ch + 1)
		renderObj_.resize(ch + 1, nullptr);

	if (!dataChanged() && !coloringChanged()) { 
		if (renderObj_[ch] = paint->redraw(renderObj_[ch], showFill_, showEdge))
			return;
	}

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

	renderObj_[ch] = paint->drawGeomColor(geom, showFill_, showEdge);
}
