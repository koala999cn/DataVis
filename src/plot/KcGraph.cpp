#include "KcGraph.h"
#include "KvPaint.h"
#include "KvData.h"
#include "KtGeometryImpl.h"
#include "KcVertexDeclaration.h"
#include <assert.h>


void KcGraph::drawImpl_(KvPaint* paint, point_getter1 getter, unsigned count, unsigned ch) const
{
	unsigned stride = count / 4096 + 1;
	point_getter1 downsamp = [getter, stride](unsigned idx) { // TODO：使用降采样算法
		return getter(stride * idx);
	};
	
	if (count > 4096)
		getter = downsamp, count /= stride;

	lineCxt_.color = majorColor(ch);
	paint->apply(lineCxt_);

	if (coloringMode() == k_one_color_solid) {
		paint->drawLineStrip(getter, count);
	}
	else { // 构建带color的vbo
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
			vtx->pos = getter(i);
			vtx->color = mapValueToColor_(vtx->pos[data()->dim()], ch); // TODO: 目前使用最高维数据插值
			vtx++;
		}

		paint->drawGeom(decl, geom, true, false);
	}
}


const color4f& KcGraph::minorColor() const
{ 
	return clrMinor_;
}


void KcGraph::setMinorColor(const color4f& minor) 
{ 
	clrMinor_ = minor;
}
