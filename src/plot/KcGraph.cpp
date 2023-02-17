#include "KcGraph.h"
#include "KvPaint.h"
#include "KvData.h"
#include "KtGeometryImpl.h"
#include <assert.h>


void KcGraph::setRenderState_(KvPaint* paint, unsigned objIdx) const
{
	paint->apply(lineCxt_);
	if (coloringMode() == k_one_color_solid)
		paint->setColor(majorColor(objIdx2ChsIdx_(objIdx)));
}


void* KcGraph::drawObjectImpl_(KvPaint* paint, GETTER getter, unsigned count, unsigned objIdx) const
{
	unsigned stride = count / 4096 + 1;
	auto downsamp = [getter, stride](unsigned idx) { // TODO：使用降采样算法
		return getter(stride * idx);
	};
	
	if (count > 4096)
		getter = downsamp, count /= stride;

	auto ch = objIdx2ChsIdx_(objIdx);
	if (coloringMode() == k_one_color_solid) 
		return paint->drawLineStrip(toPoint3Getter_(getter, ch), count); // toPoint3Getter_按需完成z值替换

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

	return paint->drawGeomColor(geom, showFill_(), showEdge_());
}


const color4f& KcGraph::minorColor() const
{ 
	return clrMinor_;
}


void KcGraph::setMinorColor_(const color4f& minor) 
{ 
	clrMinor_ = minor;
}
