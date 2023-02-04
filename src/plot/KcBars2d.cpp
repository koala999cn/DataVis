#include "KcBars2d.h"
#include "KvPaint.h"
#include "KvDiscreted.h"
#include "KvContinued.h"
#include "KtGeometryImpl.h"
#include "KuMath.h"
#include "KvSampled.h"


KcBars2d::KcBars2d(const std::string_view& name)
	: super_(name)
{
	border_.color = color4f(0, 0, 0, 1);
}


void KcBars2d::drawDiscreted_(KvPaint* paint, const KvDiscreted* disc) const
{
	bool realShowFill = showFill() && fillBrush().visible();
	bool realShowEdge = showBorder() && borderPen().visible();
	if (!realShowFill && !realShowEdge)
		return;

	auto barWidth = barWidth_(); // 目前返回dx（世界坐标）
	auto clusterWidth = barWidth * barWidthRatio_; // 每簇所占的宽度（世界坐标）
	auto easy = easyGetter_();
	auto paddingGrouped = paddingGrouped_; // 根据宽度可能调整
	auto stackWidth = (1 - paddingGrouped * (easy.groups - 1)) * clusterWidth / easy.groups;
	if (stackWidth < 0) {
		paddingGrouped = (1. - easy.groups / clusterWidth) / (easy.groups - 1);
		stackWidth = 0;
	}

	auto groupPadding = paddingGrouped * barWidth;
	auto stackPadding = paddingStacked_ * barWidth;

	struct KpVtxBuffer_
	{
		point3f pos;
		point4f clr;
	};

	auto geom = std::make_shared<KtGeometryImpl<KpVtxBuffer_, unsigned>>(k_quads);
	auto vtx = geom->newVertex(disc->size() * disc->channels() * 4);
	auto xdim = xdim_();
	auto ydim = ydim_();

	for (unsigned c = 0; c < easy.clusters; c++) {
		for (unsigned g = 0; g < easy.groups; g++) {
			auto bottom = baseLine_;
			for (unsigned s = 0; s < easy.stacks; s++) {
				auto pt = easy.getter(c, g, s);
				auto ch = stackedFirst_ ? s : g;
				auto top = bottom + pt[ydim];
				auto left = pt[xdim] - clusterWidth / 2 + g * (stackWidth + groupPadding);
				auto right = left + stackWidth;

				// 第一个顶点取right-top，这样可保证最后一个顶点为left-top（quad各顶点按顺时针排列）
				// 如此确保在flat模式下显示left-top顶点的颜色（保证按y轴插值时的正确性）
				vtx[0].pos = point3f(right, top, pt[2]);
				pt[0] = right, pt[1] = top;
				vtx[0].clr = mapValueToColor_(pt.data(), ch);

				vtx[1].pos = point3f(right, bottom + stackPadding, pt[2]);
				pt[1] = bottom;
				vtx[1].clr = mapValueToColor_(pt.data(), ch);

				vtx[2].pos = point3f(left, bottom + stackPadding, pt[2]);
				pt[0] = left;
				vtx[2].clr = mapValueToColor_(pt.data(), ch);

				vtx[3].pos = point3f(left, top, pt[2]);
				pt[1] = top;
				vtx[3].clr = mapValueToColor_(pt.data(), ch);

				vtx += 4;

				bottom = top;
			}

		}
	}

	if (realShowEdge)
		paint->apply(borderPen());

	paint->drawGeomColor(geom, realShowFill, realShowEdge);
}


KcBars2d::float_t KcBars2d::barWidth_(unsigned dim) const
{
	auto disc = discreted_();
	assert(disc && disc->size(dim) != 0);

	return disc->step(dim) != 0 ? disc->step(dim) : disc->range(dim).length() / disc->size(dim);
}


unsigned KcBars2d::xdim_() const
{
	return 0; // 
}


unsigned KcBars2d::ydim_() const
{
	unsigned ydim = 1; // 作为y轴的数据维度
	if (data()->dim() > 1) {
		ydim = 2; // 1维度作为分组数或堆叠数
		if (data()->dim() > 2 && data()->channels() == 1) // 1/2维度分别作为分组/堆叠、或堆叠/分组数
			ydim = 3;
	}

	return ydim;
}


KcBars2d::aabb_t KcBars2d::boundingBox() const
{
	auto aabb = super_::boundingBox();

	if (!empty()) {
		auto w = barWidth_();
		aabb.inflate(w / 2, 0);

		auto easy = easyGetter_();
		
		unsigned ydim = ydim_();
		if (ydim != 1) {
			auto r = data()->range(ydim);
			aabb.lower().y() = r.low();
			aabb.upper().y() = r.high();
		}

		for (unsigned c = 0; c < easy.clusters; c++) {
			for (unsigned g = 0; g < easy.groups; g++) {

				// 计算1个堆叠的长度
				float_t stackLen(0);
				for (unsigned s = 0; s < easy.stacks; s++) {
					auto pt = easy.getter(c, g, s);
					stackLen += pt[ydim];
				}

				KuMath::uniteRange(aabb.lower().y(), aabb.upper().y(),
					stackLen + baseLine_, stackLen + baseLine_);
			}
		}

		KuMath::updateRange<float_t>(aabb.lower().y(), aabb.upper().y(), baseLine_); // 确保显示baseline
	}

	return aabb;
}


unsigned KcBars2d::majorColorsNeeded() const
{
	return coloringMode() == k_colorbar_gradiant ? -1
		: (data() == nullptr ? 1 : // 数据未知，返回1
			data()->channels() > 1 ? data()->channels() // 多通道数据，返回通道数
			: discreted_()->size(1)); // 单通道数据，返回size(1)
}


const color4f& KcBars2d::minorColor() const
{
	return border_.color;
}


void KcBars2d::setMinorColor_(const color4f& minor)
{
	border_.color = minor;
}


KcBars2d::KpEasyGetter KcBars2d::easyGetter_() const
{
	assert(!empty());
	
	KpEasyGetter getter;

	auto chs = data()->channels();

	auto disc = discreted_();

	// NB: 无论哪种分组/堆叠方式，都始终保持0维度数据在x轴，簇数目等于0维度的数据量
	getter.clusters = disc->size(0);

	// 非均匀采样数据只能按通道分组，不能堆叠（无法保证x坐标一致）
	if (!disc->isSampled()) {
		getter.stacks = 1;
		getter.groups = chs;

		if (data()->dim() == 1) {
			getter.getter = [this, disc](unsigned idx, unsigned group, unsigned stack) {
				assert(stack == 0);
				auto pt = disc->pointAt(idx, group);
				pt.push_back(defaultZ(group));
				return pt;
			};
		}
		else {
			getter.getter = [this, disc](unsigned idx, unsigned group, unsigned stack) {
				assert(stack == 0);
				return disc->pointAt(idx, group);
			};
		}

		return getter;
	}


	auto samp = std::dynamic_pointer_cast<const KvSampled>(disc);

	if (chs > 1) { // 优先按通道分组/堆叠
		if (stackedFirst_) {
			getter.stacks = chs;
			getter.groups = samp->size(1);
			if (samp->dim() == 1) {
				getter.getter = [this, samp](unsigned idx, unsigned group, unsigned stack) {
					auto pt = samp->point(idx, stack);
					pt.push_back(defaultZ(stack));
					return pt;
				};
			}
			else {
				getter.getter = [samp](unsigned idx, unsigned group, unsigned stack) {
					return samp->point(idx, group, stack);
				};
			}
		}
		else {
			getter.stacks = samp->size(1);
			getter.groups = chs;
			if (samp->dim() == 1) {
				getter.getter = [this, samp](unsigned idx, unsigned group, unsigned stack) {
					auto pt = samp->point(idx, group);
					pt.push_back(defaultZ(group));
					return pt;
				};
			}
			else {
				getter.getter = [samp](unsigned idx, unsigned group, unsigned stack) {
					return samp->point(idx, stack, group);
				};
			}
		}
	}
	else if (samp->dim() == 1) { // 单通道单维度，分组数和堆叠数均为1
		getter.stacks = 1;
		getter.groups = 1;
		getter.getter = [this, samp](unsigned idx, unsigned group, unsigned stack) {
			assert(stack == 0 && group == 0);
			auto pt = samp->point(idx, 0);
			pt.push_back(defaultZ(0));
			return pt;

		};
	}
	else { // 单通道高纬度，依高维度分组/堆叠
		if (stackedFirst_) {
			getter.stacks = samp->size(1);
			getter.groups = samp->dim() > 2 ? samp->size(2) : 1;
			if (samp->dim() == 2) {
				getter.getter = [samp](unsigned idx, unsigned group, unsigned stack) {
					return samp->point(idx, stack, 0);
				};
			}
			else {
				assert(samp->dim() > 2);
				getter.getter = [samp](unsigned idx, unsigned group, unsigned stack) {
					kIndex idxes[3] = { idx, stack, group };
					return samp->point(idxes, 0);
				};
			}
		}
		else {
			getter.groups = samp->size(1);
			getter.stacks = samp->dim() > 2 ? samp->size(2) : 1;
			if (samp->dim() == 2) {
				getter.getter = [samp](unsigned idx, unsigned group, unsigned stack) {
					return samp->point(idx, group, 0);
				};
			}
			else {
				assert(samp->dim() > 2);
				getter.getter = [samp](unsigned idx, unsigned group, unsigned stack) {
					kIndex idxes[3] = { idx, group, stack };
					return samp->point(idxes, 0);
				};
			}
		}
	}

	return getter;
}
