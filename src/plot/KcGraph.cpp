#include "KcGraph.h"
#include "KvPaint.h"
#include "KvData.h"
#include "KtGeometryImpl.h"
#include <assert.h>


void KcGraph::drawImpl_(KvPaint* paint, GETTER getter, unsigned count, unsigned ch) const
{
	unsigned stride = count / 4096 + 1;
	auto downsamp = [getter, stride](unsigned idx) { // TODO：使用降采样算法
		return getter(stride * idx);
	};
	
	if (count > 4096)
		getter = downsamp, count /= stride;

	if (coloringMode() != k_colorbar_gradiant)
	    lineCxt_.color = majorColor(ch);
	paint->apply(lineCxt_);

	if (coloringMode() == k_one_color_solid) {
		paint->drawLineStrip(toPointGetter_(getter, ch), count); // toPointGetter_按需完成z值替换
		return;
	}

	// 构建带color的vbo
	struct Vertex_ {
		float3 pos;
		float4 color;
	};

	auto geom = std::make_shared<KtGeometryImpl<Vertex_>>(k_line_strip);
	auto vtx = geom->newVertex(count);
	for (unsigned i = 0; i < count; i++) {
		auto pt = getter(i);
		vtx->pos = toPoint_(pt.data(), ch);
		vtx->color = mapValueToColor_(pt.data(), ch);
		vtx++;
	}

	paint->drawGeomColor(geom, true, false);
}


const color4f& KcGraph::minorColor() const
{ 
	return clrMinor_;
}


void KcGraph::setMinorColor(const color4f& minor) 
{ 
	clrMinor_ = minor;
}
