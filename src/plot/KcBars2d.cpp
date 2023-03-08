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


void KcBars2d::setPaddingStacked(float padding)
{
	paddingStacked_ = padding;
	// TODO: if (stacks > 1)
	setDataChanged(false);
}


KcBars2d::aabb_t KcBars2d::calcBoundingBox_() const
{
	auto box = super_::calcBoundingBox_();
	KuMath::updateRange<double>(box.lower().y(), box.upper().y(), baseLine_);
	box.upper().y() += box.height() * 0.1;
	return box;
}


void* KcBars2d::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	auto barWidth = barWidthRatio_ * barWidth_(xdim());
	auto ydir = paint->projectv({ 0, 1, 0 });
	auto stackPadding = paddingStacked_ / ydir.length();

	struct KpVtxBuffer_
	{
		point3f pos;
		point4f clr;
	};

	auto disc = discreted_();
	auto geom = std::make_shared<KtGeometryImpl<KpVtxBuffer_, unsigned>>(k_quads);
	auto vtx = geom->newVertex(disc->size() * disc->channels() * 4);
	auto linesPerChannel = linesPerChannel_();

	std::vector<unsigned> stackDims;
	for (unsigned i = 0; i < disc->dim(); i++)
		if (arrangeMode(i) == k_arrange_stack)
			stackDims.push_back(i);

	for (unsigned ch = 0; ch < odata()->channels(); ch++) {
		for (unsigned idx = 0; idx < linesPerChannel; idx++) {
			auto line = lineAt_(ch, idx);

			bool floorStack(true); // 是否最底层的bar
			for (auto d : stackDims) {
				if (d == odata()->dim() - 1
					&& ch != 0) {
					floorStack = false;
					break;
				}
				else {
					auto sh = KuDataUtil::shape(*disc);
					sh.pop_back();
					if (KuDataUtil::n2index(sh, idx).at(d) != 0) {
						floorStack = false;
						break;
					}
				}
			}

			auto val = KuDataUtil::pointGetter1dAt(disc, ch, idx); // 原始数据, 只在floorStack为false时需要
			float_t offset = floorStack ? ridgeOffsetAt_(ch, idx) : 0;

			for (unsigned i = 0; i < line.size; i++) {
				auto pos = line.getter(i);
				auto pt = toPoint_(pos.data(), ch);
				auto left = pt.x();
				auto right = left + barWidth;
				auto top = pt.y();
				
				float_t bottom = floorStack ? baseLine_ + offset : top - val.getter(i).back();
				auto paddedBottom = bottom + stackPadding * KuMath::sign(top - bottom);

				// 第一个顶点取right-top，这样可保证最后一个顶点为left-top（quad各顶点按顺时针排列）
				// 如此确保在flat模式下显示left-top顶点的颜色（保证按y轴插值时的正确性）
				vtx[0].pos = point3f(right, top, pt.z());
				pos[0] = right, pos[1] = top;
				vtx[0].clr = mapValueToColor_(pos.data(), ch);

				vtx[1].pos = point3f(right, paddedBottom, pt.z());
				pos[1] = bottom;
				vtx[1].clr = mapValueToColor_(pos.data(), ch);

				vtx[2].pos = point3f(left, paddedBottom, pt.z());
				pos[0] = left;
				vtx[2].clr = mapValueToColor_(pos.data(), ch);

				vtx[3].pos = point3f(left, top, pt.z());
				pos[1] = top;
				vtx[3].clr = mapValueToColor_(pos.data(), ch);

				vtx += 4;
			}
		}
	}

	return paint->drawGeomColor(geom);
}


KcBars2d::float_t KcBars2d::barWidth_(unsigned dim) const
{
	if (dim == odata()->dim())
		return 1;

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
