#include "KcLineFilled.h"
#include "KvPaint.h"
#include "KvDiscreted.h"
#include "KtGeometryImpl.h"
#include "KtLineS2d.h"
#include "KuDataUtil.h"
#include <assert.h>


unsigned KcLineFilled::majorColorsNeeded() const
{
	switch (fillMode_) {
	case k_fill_between:
		return linesTotal_() - 1;

	case k_fill_delta:
		return linesTotal_() / 2;

	default:
		break;
	}
	return linesTotal_();
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
	if (objIdx == 1) { // edge״̬����
		paint->apply(lineCxt_);
	}
	else {
		paint->setEdged(false); // ʼ�ղ����
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

		unsigned idx(0);
		for (kIndex ch = 0; ch < odata()->channels(); ch++) {
			for (unsigned i = 0; i < linesPerChannel_(); i++) {
				if (fillMode_ != k_fill_delta || idx % 2 == 0) {
					auto g = lineAt_(ch, i);
					fns.push_back(toPoint3Getter_(g.getter, ch));
					cnts.push_back(g.size);
				}

				idx++;
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
		else if (fillMode_ == k_fill_delta)
			return fillDelta_(paint);
		else if (fillMode_ == k_fill_ridge)
			return fillRidge_(paint);

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
	// NB: �������ͼû��ʹ��solid geom���ƣ�������ɫ�仯���ع�����
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
		setStackMode_(mode == k_fill_stacked ? k_stack_channel : k_stack_none);
		setRidgeMode_(mode == k_fill_ridge ? k_ridge_channel : k_ridge_none);
		setDataChanged(false);
	}
}


void KcLineFilled::setBaseMode(KeBaseMode mode)
{
	if (mode != baseMode_) {
		baseMode_ = mode;
		setDataChanged(false);
	}
}


void KcLineFilled::setBaseLine(float_t base)
{
	baseLine_ = base;
	setDataChanged(false);
}


void KcLineFilled::setBasePoint(const point3& pt)
{
	basePoint_ = pt;
	setDataChanged(false);
}


void* KcLineFilled::fillOverlay_(KvPaint* paint) const
{
	auto geom = std::make_shared<KtGeometryImpl<kPrivate::KpVertex_, unsigned>>(k_triangles);
	unsigned idx(0); // ���ڻ�ȡ��ɫ

	for (kIndex ch = 0; ch < odata()->channels(); ch++) {
		for (unsigned i = 0; i < linesPerChannel_(); i++) {
			auto g1 = lineAt_(ch, i);

			if (baseMode_ != k_base_point) {
				auto g2 = baseGetter_(ch, i, g1.getter);
				auto vtx = geom->newVertex((g1.size - 1) * 6); // ÿ���������2�������Σ���6������
				fillBetween_(paint, g1.getter, g2, g1.size, idx++, vtx);
			}
			else {
				auto vtx = geom->newVertex((g1.size - 1) * 3);
				fillBetween_(paint, basePointAt_(ch, i), g1.getter, g1.size, idx++, vtx);
			}
		}
	}

	return paint->drawGeomColor(geom);
}


void* KcLineFilled::fillBetween_(KvPaint* paint, bool baseline) const
{
	auto geom = std::make_shared<KtGeometryImpl<kPrivate::KpVertex_, unsigned>>(k_triangles);

	unsigned idx(0); // ���ڻ�ȡ��ɫ

	if (baseline) {
		auto g1 = lineAt_(0, 0);

		if (baseMode_ != k_base_point) {
			auto g2 = baseGetter_(0, 0, g1.getter);
			auto vtx = geom->newVertex((g1.size - 1) * 6); // ÿ���������2�������Σ���6������
			fillBetween_(paint, g1.getter, g2, g1.size, idx++, vtx);
		}
		else {
			auto vtx = geom->newVertex((g1.size - 1) * 3);
			fillBetween_(paint, basePointAt_(0, 0), g1.getter, g1.size, idx++, vtx);
		}
	}


	GETTER g1 = nullptr;
	for (kIndex ch = 0; ch < odata()->channels(); ch++) {
		for (unsigned i = 0; i < linesPerChannel_(); i++) {
			auto g2 = lineAt_(ch, i);

			if (g1) {
				auto vtx = geom->newVertex((g2.size - 1) * 6); // ÿ���������2�������Σ���6������
				fillBetween_(paint, g1, g2.getter, g2.size, idx++, vtx);
			}

			g1 = g2.getter;
		}
	}

	return paint->drawGeomColor(geom);
}


KcLineFilled::GETTER KcLineFilled::baseGetter_(unsigned ch, unsigned idx, GETTER g) const
{
	auto offset = ridgeOffsetAt_(ch, idx);

	return [g, this, offset](unsigned i) {
		auto pt = g(i);
		if (baseMode_ == k_base_xline)
			pt[1] = baseLine_;
		else
			pt[0] = baseLine_;
		pt[1] += offset;
		return pt;
	};
}


KcLineFilled::point3 KcLineFilled::basePointAt_(unsigned ch, unsigned idx) const
{
	if (fillMode_ != k_fill_ridge)
		return basePoint_;

	auto pt = basePoint_;
	pt[1] += ridgeOffsetAt_(ch, idx);
	return pt;
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
		if (pt) { // �ཻ

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
			vtx[5] = vtx[2]; // TODO: flatģʽ�£�Ŀǰʹ���е����ݻ��ƣ��������ݵ�

		}
		else { // ���ཻ
			vtx[0].pos = toPoint_(p01.data(), ch);
			vtx[0].clr = mapValueToColor_(p01.data(), ch);
			vtx[1].pos = toPoint_(p00.data(), ch);
			vtx[1].clr = mapValueToColor_(p00.data(), ch);
			vtx[2].pos = toPoint_(p10.data(), ch);
			vtx[2].clr = mapValueToColor_(p10.data(), ch);

			vtx[3].pos = toPoint_(p11.data(), ch);
			vtx[3].clr = mapValueToColor_(p11.data(), ch);
			vtx[4] = vtx[0];
			vtx[5] = vtx[2]; // �������һ����������һ��������һ�£�����Ӧflat��Ⱦģʽ
			                 // TODO: ʹ��getter1����getter2������Ϊfalt�Ļ���ɫ
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


void* KcLineFilled::fillDelta_(KvPaint* paint) const
{
	auto geom = std::make_shared<KtGeometryImpl<kPrivate::KpVertex_, unsigned>>(k_triangles);

	unsigned idx(0); // ���ڻ�ȡ��ɫ

	GETTER g1 = nullptr;
	for (kIndex ch = 0; ch < odata()->channels(); ch++) {
		for (unsigned i = 0; i < linesPerChannel_(); i++) {
			auto g2 = lineAt_(ch, i);

			if (g1) {
				auto vtx = geom->newVertex((g2.size - 1) * 6); // ÿ���������2�������Σ���6������

				auto g1Real = [g1, g2](unsigned i) {
					auto r1 = g1(i);
					auto r2 = g2.getter(i);
					r1.back() -= r2.back();
					return r1;
				};

				auto g2Real = [g1, g2](unsigned i) {
					auto r1 = g1(i);
					auto r2 = g2.getter(i);
					r1.back() += r2.back();
					return r1;
				};

				fillBetween_(paint, g1Real, g2Real, g2.size, idx++, vtx);
				g1 = nullptr;
			}
			else {
				g1 = g2.getter;
			}
		}
	}

	return paint->drawGeomColor(geom);
}


void* KcLineFilled::fillRidge_(KvPaint* paint) const
{
	return fillOverlay_(paint);
}

