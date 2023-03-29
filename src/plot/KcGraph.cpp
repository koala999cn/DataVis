#include "KcGraph.h"
#include "KvPaint.h"
#include "KvData.h"
#include "KtGeometryImpl.h"
#include <assert.h>


const color4f& KcGraph::minorColor() const
{
	return clrMinor_;
}


void KcGraph::setMinorColor_(const color4f& minor)
{
	clrMinor_ = minor;
}


unsigned KcGraph::objectCount() const
{
	return empty() ? 0 : odata()->channels();
}


bool KcGraph::objectVisible_(unsigned objIdx) const
{
	return lineCxt_.style != KpPen::k_none;
}


void KcGraph::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	paint->apply(lineCxt_);
	if (coloringMode() == k_one_color_solid)
		paint->setColor(majorColor(objIdx));
}


namespace kPrivate
{
	KuDataUtil::KpPointGetter1d downSampling_(const KuDataUtil::KpPointGetter1d& g)
	{
		if (g.size < 4096)
			return g;

		unsigned stride = g.size / 4096 + 1;
		auto downsamp = [g, stride](unsigned idx) { // TODO：使用降采样算法
			return g.getter(stride * idx);
		};

		return { downsamp, g.size / stride };
	}
}


void* KcGraph::drawObject_(KvPaint* paint, unsigned ch) const
{
	auto g = linesAt_(ch);

	if (coloringMode() == k_one_color_solid) {
		return paint->drawLineStrip(toPoint3Getter_(g.getter, ch), g.size);
	}
	else {
		// 构建带color的vbo
		struct Vertex_ {
			float3 pos;
			float4 color;
		};

		auto geom = std::make_shared<KtGeometryImpl<Vertex_>>(k_line_strip);
		auto vtx = geom->newVertex(g.size);

		for (unsigned i = 0; i < g.size; i++) {
			auto pt = g.getter(i);
			vtx->pos = toPoint_(pt.data(), ch);
			vtx->color = mapValueToColor_(pt.data(), ch);
			vtx++;
		}

		return paint->drawGeomColor(geom);
	}
}


KuDataUtil::KpPointGetter1d KcGraph::linesAt_(unsigned ch) const
{
	auto lineSize = sizePerLine_();
	std::vector<GETTER> lines(linesPerChannel_());

	for (unsigned i = 0; i < lines.size(); i++) {
		auto g = lineAt_(ch, i);
		assert(lineSize == g.size);
		lines[i] = g.getter;
	}

	std::vector<kReal> nan(odim() + 1, KuMath::nan<kReal>());
	KuDataUtil::KpPointGetter1d g;
	g.size = lines.size() * (lineSize + 1) - 1; // 有linesPerChannel_-1个nan
	g.getter = [lines, lineSize, nan](unsigned idx) {
		auto i = idx % (lineSize + 1);
		return i == lineSize ? nan : lines[idx / (lineSize + 1)](i);
	};

	return g;
}
