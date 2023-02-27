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


void KcLineFilled::setBaseMode(KeBaseMode mode)
{
	if (mode != baseMode_) {
		baseMode_ = mode;
		setDataChanged();
	}
}


void KcLineFilled::setBaseLine(float_t base)
{
	baseLine_ = base;
	setDataChanged();
}


void KcLineFilled::setBasePoint(const point3& pt)
{
	basePoint_ = pt;
	setDataChanged();
}


void* KcLineFilled::fillOverlay_(KvPaint* paint) const
{
	auto geom = std::make_shared<KtGeometryImpl<kPrivate::KpVertex_, unsigned>>(k_triangles);
	unsigned idx(0); // 用于获取主色

	for (kIndex ch = 0; ch < data()->channels(); ch++) {
		for (unsigned i = 0; i < linesPerChannel_(); i++) {
			auto g1 = lineAt_(ch, i);

			if (baseMode_ != k_base_point) {
				auto g2 = baseGetter_(g1.getter);
				auto vtx = geom->newVertex((g1.size - 1) * 6); // 每个区间绘制2个三角形，共6个顶点
				fillBetween_(paint, g1.getter, g2, g1.size, idx++, vtx);
			}
			else {
				auto vtx = geom->newVertex((g1.size - 1) * 3);
				fillBetween_(paint, basePoint_, g1.getter, g1.size, idx++, vtx);
			}
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

		if (baseMode_ != k_base_point) {
			auto g2 = baseGetter_(g1.getter);
			auto vtx = geom->newVertex((g1.size - 1) * 6); // 每个区间绘制2个三角形，共6个顶点
			fillBetween_(paint, g1.getter, g2, g1.size, 0, vtx);
		}
		else {
			auto vtx = geom->newVertex((g1.size - 1) * 3);
			fillBetween_(paint, basePoint_, g1.getter, g1.size, 0, vtx);
		}
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


KcLineFilled::GETTER KcLineFilled::baseGetter_(GETTER g) const
{
	return [g, this](unsigned i) {
		auto pt = g(i);
		if (baseMode_ == k_base_xline)
			pt[1] = baseLine_;
		else
			pt[0] = baseLine_;
		return pt;
	};
}


void KcLineFilled::fillBetween_(KvPaint* paint, GETTER getter1, GETTER getter2, 
	unsigned count, unsigned ch, void* buf) const
{
	assert(count > 0);
	auto vtx = (kPrivate::KpVertex_*)buf;

	auto p00 = getter1(0);
	auto p01 = getter2(0);

	for (unsigned i = 1; i < count; i++) {
		auto p10 = getter1(i);
		auto p11 = getter2(i);

		using point2 = KtPoint<float_t, 2>;
		KtLineS2d<float_t> ln0({ p00[0], p00[1] }, { p10[0], p10[1] });
		KtLineS2d<float_t> ln1({ p01[0], p01[1] }, { p11[0], p11[1] });
		auto pt = ln0.intersects(ln1);
		if (pt) { // 相交

			vtx[0].pos = toPoint_(p01.data(), ch);
			vtx[0].clr = mapValueToColor_(p01.data(), ch);
			vtx[1].pos = toPoint_(p00.data(), ch);
			vtx[1].clr = mapValueToColor_(p00.data(), ch);

			point3 ptm(pt->x(), pt->y(), vtx[0].pos.z());
			vtx[2].pos = toPoint_(ptm.data(), ch);
			vtx[2].clr = mapValueToColor_(ptm.data(), ch);

			vtx[3].pos = toPoint_(p11.data(), ch);
			vtx[3].clr = mapValueToColor_(p11.data(), ch);
			vtx[4].pos = toPoint_(p10.data(), ch);
			vtx[4].clr = mapValueToColor_(p10.data(), ch);
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


void KcLineFilled::fillBetween_(KvPaint* paint, const point3& pt, GETTER g, 
	unsigned count, unsigned ch, void* buf) const
{
	assert(count > 0);
	auto vtx = (kPrivate::KpVertex_*)buf;

	auto pc = toPoint_(pt.data(), ch);
	auto cc = mapValueToColor_(pt.data(), ch);
	auto p0 = toPoint_(g(0).data(), ch);
	auto c0 = mapValueToColor_(p0.data(), ch);

	for (unsigned i = 1; i < count; i++) {
		auto p1 = g(i);

		vtx[0].pos = pc;
		vtx[0].clr = cc;
		vtx[1].pos = p0;
		vtx[1].clr = c0;
		vtx[2].pos = toPoint_(p1.data(), ch);
		vtx[2].clr = mapValueToColor_(p1.data(), ch);

		p0 = vtx[2].pos;
		c0 = vtx[2].clr;
		vtx += 3;
	}
}
