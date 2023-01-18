#include "KcGraph.h"
#include "KvPaint.h"
#include "KvData.h"
#include "KtGeometryImpl.h"
#include "KcVertexDeclaration.h"
#include <assert.h>


void KcGraph::drawImpl_(KvPaint* paint, GETTER getter, unsigned count, unsigned ch) const
{
	unsigned stride = count / 4096 + 1;
	auto downsamp = [getter, stride](unsigned idx) { // TODO��ʹ�ý������㷨
		return getter(stride * idx);
	};
	
	if (count > 4096)
		getter = downsamp, count /= stride;

	lineCxt_.color = majorColor(ch);
	paint->apply(lineCxt_);

	if (coloringMode() == k_one_color_solid) {
		paint->drawLineStrip(toPaintGetter(getter, ch), count); // toPaintGetter�������zֵ�滻
		return;
	}

	// ������color��vbo
	struct Vertex_ {
		float3 pos;
		float4 color;
	};

	auto decl = std::make_shared<KcVertexDeclaration>();
	decl->pushAttribute(KcVertexAttribute::k_float3, KcVertexAttribute::k_position);
	decl->pushAttribute(KcVertexAttribute::k_float4, KcVertexAttribute::k_diffuse);

	auto geom = std::make_shared<KtGeometryImpl<Vertex_>>(k_line_strip);
	auto vtx = geom->newVertex(count);
	for (unsigned i = 0; i < count; i++) {
		auto pt = getter(i);
		vtx->pos = toPoint_(pt.data(), ch);
		vtx->color = mapValueToColor_(pt.data(), ch);
		vtx++;
	}

	paint->drawGeom(decl, geom, true, false);
}


const color4f& KcGraph::minorColor() const
{ 
	return clrMinor_;
}


void KcGraph::setMinorColor(const color4f& minor) 
{ 
	clrMinor_ = minor;
}
