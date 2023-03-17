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


void* KcGraph::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	std::vector<KvPaint::point_getter1> fns;
	std::vector<unsigned> cnts;
	fns.reserve(linesTotal_());
	cnts.reserve(linesTotal_());

	unsigned idx(0);
	for (unsigned i = 0; i < linesPerChannel_(); i++) {
		auto g = kPrivate::downSampling_(lineAt_(objIdx, i));
		fns.push_back(toPoint3Getter_(g.getter, objIdx)); // toPoint3Getter_按需完成z值替换
		cnts.push_back(g.size);
	}

	if (coloringMode() == k_one_color_solid) {
		return paint->drawLineStrips(fns, cnts);
	}
	else {
		// 构建带color的vbo
		struct Vertex_ {
			float3 pos;
			float4 color;
		};

		auto geom = std::make_shared<KtGeometryImpl<Vertex_>>(k_line_strip);
		unsigned total(0);
		for (auto& i : cnts)
			total += i;
		total += cnts.size() - 1;
	
		auto vtx = geom->newVertex(total);

		for (unsigned i = 0; i < cnts.size(); i++) {
			for (unsigned j = 0; j < cnts[i]; j++) {
				auto pt = fns[i](j);
				vtx->pos = toPoint_(pt.data(), objIdx);
				vtx->color = mapValueToColor_(pt.data(), objIdx);
				vtx++;
			}

			if (i != cnts.size() - 1) {
				vtx->pos = float3(KuMath::nan<float>());
				vtx++;
			}
		}

		return paint->drawGeomColor(geom);
	}
}


KuDataUtil::KpPointGetter1d KcGraph::linesAtChannel_(unsigned ch) const
{
	auto lineSize = sizePerLine_();
	std::vector<GETTER> lines;

	for (unsigned i = 0; i < linesPerChannel_(); i++) {
		auto g = lineAt_(ch, i);
		assert(lineSize == g.size);
		lines.push_back(g.getter);
	}

	KuDataUtil::KpPointGetter1d g;
	g.size = linesPerChannel_() * (lineSize + 1) - 1; // 有linesPerChannel_-1个nan
	g.getter = [lines, lineSize](unsigned idx) {
		return lines.front()(idx);
	};

	return g;
}

