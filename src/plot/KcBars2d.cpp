#include "KcBars2d.h"
#include "KvPaint.h"
#include "KtGeometryImpl.h"
#include "KvDiscreted.h"
#include "KuMath.h"


KcBars2d::KcBars2d(const std::string_view& name)
	: super_(name)
{
	border_.color = color4f(0, 0, 0, 1);
}


unsigned KcBars2d::objectCount() const
{
	return 1;
}


void KcBars2d::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	paint->setFilled(realFilled_());
	paint->setEdged(realEdged_());
	if (realEdged_())
		paint->apply(borderPen());
}


bool KcBars2d::objectVisible_(unsigned objIdx) const
{
	return realFilled_() || realEdged_();
}


bool KcBars2d::realFilled_() const
{
	return showFill() && fillBrush().visible();
}


bool KcBars2d::realEdged_() const
{
	return showBorder() && borderPen().visible();
}


void KcBars2d::setBarWidthRatio(float w)
{
	barWidthRatio_ = w;
	setDataChanged(false);
}


void KcBars2d::setBaseLine(float base)
{
	baseLine_ = base;
	setDataChanged(false);
	setBoundingBoxExpired_();
}


void KcBars2d::setStackPadding(float padding)
{
	stackPadding_ = padding;
	// TODO: if (stacks > 1)
	setDataChanged(false);
}


KcBars2d::aabb_t KcBars2d::calcBoundingBox_() const
{
	auto box = super_::calcBoundingBox_();

	// 修正x轴
	auto xw = barWidth_(xdim());
	box.lower().x() -= xw; box.upper().x() += xw;

	// 修正y轴
	KuMath::updateRange<double>(box.lower().y(), box.upper().y(), baseLine_);
	box.upper().y() += box.height() * 0.1;

	return box;
}


namespace kPrivate
{
	struct KpVertexPC
	{
		point3f pos;
		point4f clr;
	};
}

void* KcBars2d::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	auto stackPadding = stackPadding_ / paint->projectv({ 0, 1, 0 }).length();

	auto disc = discreted_();
	auto linesPerChannel = linesPerChannel_();
	auto vtxSize = vtxSizePerBar_();
	auto geom = std::make_shared<KtGeometryImpl<kPrivate::KpVertexPC>>(k_quads);
	geom->reserve(disc->size() * disc->channels() * vtxSize.first, 
		disc->size() * disc->channels() * vtxSize.second);

	for (unsigned ch = 0; ch < odata()->channels(); ch++) {
		for (unsigned idx = 0; idx < linesPerChannel; idx++) {
			auto line = lineAt_(ch, idx);
			bool floorStack = isFloorStack_(ch, idx);

			KuDataUtil::KpPointGetter1d val;
			float_t offset(0);
			if (!floorStack)
				val = KuDataUtil::pointGetter1dAt(disc, ch, idx); // 原始数据, 只在floorStack为false时需要
			else
				offset = ridgeOffsetAt_(ch, idx); // 底部stack的偏移 只在floorStack为true时需要

			for (unsigned i = 0; i < line.size; i++) {
				auto pos = line.getter(i);
				auto pt = toPoint_(pos.data(), ch); // TODO: 此处多了一次toPoint_调用
				auto top = pt.y();
				
				float_t bottom = floorStack ? baseLine_ + offset : top - val.getter(i).back();
				auto paddedBottom = bottom + stackPadding * KuMath::sign(top - bottom);

				auto idxBase = geom->vertexCount();
				auto vtx = geom->newVertex(vtxSize.first);
				auto index = geom->newIndex(vtxSize.second);
				drawOneBar_(pos.data(), ch, paddedBottom, vtx, index, idxBase);
			}
		}
	}

	return paint->drawGeomColor(geom);
}


std::pair<unsigned, unsigned> KcBars2d::vtxSizePerBar_() const
{
	return { 4, 0 }; // 每个bar有4个顶点，无索引
}


void KcBars2d::drawOneBar_(float_t* pos, unsigned ch, float_t bottom, void* vtxBuf, void*, unsigned) const
{
	auto vtx = (kPrivate::KpVertexPC*)vtxBuf;

	auto barWidth = barWidthRatio_ * barWidth_(xdim());
	auto pt = toPoint_(pos, ch);
	auto left = pt.x();
	auto right = left + barWidth;
	auto top = pt.y();

	// 第一个顶点取right-top，这样可保证最后一个顶点为left-top（quad各顶点按顺时针排列）
	// 如此确保在flat模式下显示left-top顶点的颜色（保证按y轴插值时的正确性）
	vtx[0].pos = point3f(right, top, pt.z());
	pos[xdim()] = right, pos[ydim()] = top;
	vtx[0].clr = mapValueToColor_(pos, ch);

	vtx[1].pos = point3f(right, bottom, pt.z());
	pos[ydim()] = bottom;
	vtx[1].clr = mapValueToColor_(pos, ch);

	vtx[2].pos = point3f(left, bottom, pt.z());
	pos[xdim()] = left;
	vtx[2].clr = mapValueToColor_(pos, ch);

	vtx[3].pos = point3f(left, top, pt.z());
	pos[ydim()] = top;
	vtx[3].clr = mapValueToColor_(pos, ch);
}


KcBars2d::float_t KcBars2d::barWidth_(unsigned dim) const
{
	if ((usingDefaultZ_() && dim == zdim()) || dim >= odata()->dim())
		return stepZ() == 0 ? 1 : stepZ();

	auto disc = discreted_();
	assert(disc && disc->size(dim) != 0);

	return disc->step(dim) != 0 ? disc->step(dim) : disc->range(dim).length() / disc->size(dim);
}


const color4f& KcBars2d::minorColor() const
{
	return border_.color;
}


void KcBars2d::setMinorColor_(const color4f& minor)
{
	border_.color = minor;
}
