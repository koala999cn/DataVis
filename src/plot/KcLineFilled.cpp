#include "KcLineFilled.h"
#include "KvPaint.h"
#include "KvDiscreted.h"
#include "KtGeometryImpl.h"
#include "KtLineS2d.h"
#include "KuDataUtil.h"
#include <assert.h>


unsigned KcLineFilled::majorColorsNeeded() const
{
	return fillMode_ == k_fill_between ? linesTotal_() - 1 : linesTotal_();
}


bool KcLineFilled::objectVisible_(unsigned objIdx) const
{
	if (objIdx & 1)
		return showLine_ && lineCxt_.visible();
	else 
		return fillCxt_.style != KpBrush::k_none;
}


void KcLineFilled::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	if (objIdx == 1) { // edge状态设置
		paint->apply(lineCxt_);
	}
	else {
		paint->setEdged(false); // 始终不描边
		paint->setFilled(true);
		paint->apply(fillCxt_);
	}
}


namespace kPrivate
{
	struct KpVertex_
	{
		point3f pos;
		point4f clr;
	};
}

void* KcLineFilled::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	if (objIdx == 1) {
		std::vector<KvPaint::point_getter1> fns;
		std::vector<unsigned> cnts;
		fns.reserve(linesTotal_());
		cnts.reserve(linesTotal_());

		for (kIndex ch = 0; ch < data()->channels(); ch++) {
			for (unsigned i = 0; i < linesPerChannel_(); i++) {
				auto g = lineAt_(ch, i);
				fns.push_back(toPoint3Getter_(g.getter, ch));
				cnts.push_back(g.size);
			}
		}

		return paint->drawLineStrips(fns, cnts);
	}
	else {
		if (fillMode_ == k_fill_overlay)
			return fillOverlay_(paint);
		else if (fillMode_ == k_fill_stacked)
			return fillBetween_(paint, true);
		else if (fillMode_ == k_fill_between)
			return fillBetween_(paint, false);

		assert(false);
		return nullptr;
	}
}


const color4f& KcLineFilled::minorColor() const
{
	return lineCxt_.color;
}


bool KcLineFilled::objectReusable_(unsigned objIdx) const
{
	// NB: 由于面积图没有使用solid geom绘制，所以颜色变化须重构对象
	return !dataChanged() && !coloringChanged();
}


void KcLineFilled::setMinorColor_(const color4f& minor)
{
	lineCxt_.color = minor;
}


void KcLineFilled::setFillMode(KeFillMode mode)
{
	if (mode != fillMode_) {
		fillMode_ = mode;
		setStackMode_(mode == k_fill_stacked);
		setDataChanged();
	}
}


void* KcLineFilled::fillOverlay_(KvPaint* paint) const
{
	auto geom = std::make_shared<KtGeometryImpl<kPrivate::KpVertex_, unsigned>>(k_triangles);

	for (kIndex ch = 0; ch < data()->channels(); ch++) {
		for (unsigned i = 0; i < linesPerChannel_(); i++) {
			auto g1 = lineAt_(ch, i);
			auto g2 = [g1](unsigned i) {
				auto pt = g1.getter(i);
				pt[1] = 0; // TODO: 暂时取y=0基线
				return pt;
			};

			auto vtx = geom->newVertex((g1.size - 1) * 6); // 每个区间绘制2个三角形，共6个顶点

			fillBetween_(paint, g1.getter, g2, g1.size, ch, vtx);
		}
	}

	return paint->drawGeomColor(geom);
}


void* KcLineFilled::fillBetween_(KvPaint* paint, bool baseline) const
{
	auto geom = std::make_shared<KtGeometryImpl<kPrivate::KpVertex_, unsigned>>(k_triangles);

	unsigned idx(0); // 用于获取主色

	if (baseline) {
		auto g1 = lineAt_(0, 0);
		auto g2 = [g1](unsigned i) {
			auto pt = g1.getter(i);
			pt[1] = 0; // TODO: 暂时取y=0基线
			return pt;
		};

		auto vtx = geom->newVertex((g1.size - 1) * 6); // 每个区间绘制2个三角形，共6个顶点

		fillBetween_(paint, g1.getter, g2, g1.size, idx++, vtx);
	}


	GETTER g1 = nullptr;
	for (kIndex ch = 0; ch < data()->channels(); ch++) {
		for (unsigned i = 0; i < linesPerChannel_(); i++) {
			auto g2 = lineAt_(ch, i);

			if (g1) {
				auto vtx = geom->newVertex((g2.size - 1) * 6); // 每个区间绘制2个三角形，共6个顶点
				fillBetween_(paint, g1, g2.getter, g2.size, idx++, vtx);
			}

			g1 = g2.getter;
		}
	}

	return paint->drawGeomColor(geom);
}


void KcLineFilled::fillBetween_(KvPaint* paint, GETTER getter1, GETTER getter2, 
	unsigned count, unsigned ch, void* buf) const
{
	assert(count > 0);
	auto vtx = (kPrivate::KpVertex_*)buf;

	auto p00 = getter1(0);
	auto p01 = getter2(0);

	// assert(p00[2] == p01[2]); // 要求各点都在一个z平面上

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
}
