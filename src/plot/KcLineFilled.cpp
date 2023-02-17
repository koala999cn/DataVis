#include "KcLineFilled.h"
#include "KvPaint.h"
#include "KvData.h"
#include "KtGeometryImpl.h"
#include "KtLineS2d.h"
#include <assert.h>


bool KcLineFilled::showFill_() const 
{ 
	return fillCxt_.style != KpBrush::k_none;
}


bool KcLineFilled::showEdge_() const 
{ 
	return showLine_ && lineCxt_.visible();
}


void KcLineFilled::setRenderState_(KvPaint* paint, unsigned objIdx) const
{
	if (objIdx & 1) { // edge状态设置
		paint->apply(lineCxt_);
	}
	else if (coloringMode() == k_one_color_solid) { // fill状态设置
		paint->apply(fillCxt_);
		paint->setColor(majorColor(objIdx2ChsIdx_(objIdx)));
	}
}


void* KcLineFilled::drawObjectImpl_(KvPaint* paint, GETTER getter, unsigned count, unsigned objIdx) const
{
	unsigned stride = count / 4096 + 1;
	auto downsamp = [getter, stride](unsigned idx) { // TODO：使用降采样算法
		return getter(stride * idx);
	};

	if (count > 4096)
		getter = downsamp, count /= stride;

	auto ch = objIdx2ChsIdx_(objIdx);

	if (objIdx & 1) {
		return showEdge_() ? paint->drawLineStrip(toPoint3Getter_(getter, ch), count) : nullptr;
	}
	else if (showFill_()) {

		auto getter2 = [getter](unsigned i) {
			auto pt = getter(i);
			pt[1] = 0; // TODO: 暂时取y=0基线
			return pt;
		};

		if (coloringMode() == k_one_color_solid) {
			return paint->fillBetween(toPoint3Getter_(getter, ch), toPoint3Getter_(getter2, ch), count);
		}
		else {
			return fillGradiant_(paint, getter, getter2, count, ch);
		}
	}

	return nullptr;
}


const color4f& KcLineFilled::minorColor() const
{
	return lineCxt_.color;
}


void KcLineFilled::setMinorColor_(const color4f& minor)
{
	lineCxt_.color = minor;
}


void* KcLineFilled::fillGradiant_(KvPaint* paint, GETTER getter1, GETTER getter2, unsigned count, unsigned ch) const
{
	assert(count > 0);

	struct KpVertex_
	{
		point3f pos;
		point4f clr;
	};

	auto geom = std::make_shared<KtGeometryImpl<KpVertex_, unsigned>>(k_triangles);
	auto vtx = geom->newVertex((count - 1) * 6); // 每个区间绘制2个三角形，共6个顶点

	auto p00 = getter1(0);
	auto p01 = getter2(0);

	assert(p00[2] == p01[2]); // 要求各点都在一个z平面上

	for (unsigned i = 1; i < count; i++) {
		auto p10 = getter1(i);
		auto p11 = getter2(i);

		using point2 = KtPoint<float_t, 2>;
		KtLineS2d<float_t> ln0((const point2&)p00, (const point2&)p10);
		KtLineS2d<float_t> ln1((const point2&)p01, (const point2&)p11);
		auto pt = ln0.intersects(ln1);
		if (pt) { // 相交

			point3 ptm(pt->x(), pt->y(), p00[2]);

			vtx[0].pos = toPoint_(p01.data(), ch);
			vtx[0].clr = mapValueToColor_(p01.data(), ch);
			vtx[1].pos = toPoint_(p00.data(), ch);
			vtx[1].clr = mapValueToColor_(p00.data(), ch);
			vtx[2].pos = toPoint_(ptm.data(), ch);
			vtx[2].clr = mapValueToColor_(ptm.data(), ch);

			vtx[3].pos = toPoint_(p10.data(), ch);
			vtx[3].clr = mapValueToColor_(p10.data(), ch);
			vtx[4].pos = toPoint_(p11.data(), ch);
			vtx[4].clr = mapValueToColor_(p11.data(), ch);
			vtx[5] = vtx[2]; // TODO: flat模式下，目前使用中点数据绘制，而非数据点

		}
		else { // 不相交
			vtx[0].pos = toPoint_(p01.data(), ch);
			vtx[0].clr = mapValueToColor_(p01.data(), ch);
			vtx[1].pos = toPoint_(p00.data(), ch);
			vtx[1].clr = mapValueToColor_(p00.data(), ch);
			vtx[2].pos = toPoint_(p10.data(), ch);
			vtx[2].clr = mapValueToColor_(p10.data(), ch);

			vtx[3].pos = toPoint_(p11.data(), ch);
			vtx[3].clr = mapValueToColor_(p11.data(), ch);
			vtx[4] = vtx[0];
			vtx[5] = vtx[2]; // 保持最后一个顶点与另一个三角形一致，以适应flat渲染模式
			                 // TODO: 使用getter1还是getter2数据作为falt的绘制色
		}

		p00 = p10, p01 = p11;
		vtx += 6;
	}

	return paint->drawGeomColor(geom, true, false);
}
