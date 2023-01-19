#include "KcLineFilled.h"
#include "KvPaint.h"
#include "KvData.h"
#include "KtGeometryImpl.h"
#include "KtLineS2d.h"
#include <assert.h>


void KcLineFilled::drawImpl_(KvPaint* paint, GETTER getter, unsigned count, unsigned ch) const
{
	unsigned stride = count / 4096 + 1;
	auto downsamp = [getter, stride](unsigned idx) { // TODO：使用降采样算法
		return getter(stride * idx);
	};

	if (count > 4096)
		getter = downsamp, count /= stride;

	auto getter2 = [getter](unsigned i) {
		auto pt = getter(i);
		pt[1] = 0; // TODO: 暂时取y=0基线
		return pt;
	};

	if (fillCxt_.style != KpBrush::k_none) {
		if (coloringMode() == k_one_color_solid) {
			fillCxt_.color = majorColor(ch);
			paint->apply(fillCxt_);
			paint->fillBetween(toPointGetter_(getter, ch), toPointGetter_(getter2, ch), count);
		}
		else {
			fillGradiant_(paint, getter, getter2, count, ch);
		}
	}

	if (showLine_ && lineCxt_.visible()) {
		paint->apply(lineCxt_);
		paint->drawLineStrip(toPointGetter_(getter, ch), count);
	}
}


const color4f& KcLineFilled::minorColor() const
{
	return lineCxt_.color;
}


void KcLineFilled::setMinorColor(const color4f& minor)
{
	lineCxt_.color = minor;
}


void KcLineFilled::fillGradiant_(KvPaint* paint, GETTER getter1, GETTER getter2, unsigned count, unsigned ch) const
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
			vtx[5] = vtx[2];

		}
		else { // 不相交
			vtx[0].pos = toPoint_(p01.data(), ch);
			vtx[0].clr = mapValueToColor_(p01.data(), ch);
			vtx[1].pos = toPoint_(p00.data(), ch);
			vtx[1].clr = mapValueToColor_(p00.data(), ch);
			vtx[2].pos = toPoint_(p10.data(), ch);
			vtx[2].clr = mapValueToColor_(p10.data(), ch);

			vtx[3] = vtx[2];
			vtx[4].pos = toPoint_(p11.data(), ch);
			vtx[4].clr = mapValueToColor_(p11.data(), ch);
			vtx[5] = vtx[0];
		}

		p00 = p10, p01 = p11;
		vtx += 6;
	}

	paint->drawGeomColor(geom, true, false);
}
