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
}


void KcBars2d::setStackedFirst(bool b)
{
	stackedFirst_ = b;
	setDataChanged(false); // TODO: check it
}


void KcBars2d::setPaddingStacked(float padding)
{
	paddingStacked_ = padding;
	// TODO: if (stacks > 1)
	setDataChanged(false);
}


void KcBars2d::setPaddingGrouped(float padding)
{
	paddingGrouped_ = padding;
	// TODO: if (groups > 1)
	setDataChanged(false);
}


void* KcBars2d::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	auto barWidth = barWidth_(); // Ŀǰ����dx���������꣩
	auto clusterWidth = barWidth * barWidthRatio_; // ÿ����ռ�Ŀ�ȣ��������꣩
	auto easy = easyGetter_();
	auto paddingGrouped = paddingGrouped_; // ���ݿ�ȿ��ܵ���
	auto stackWidth = (1 - paddingGrouped * (easy.groups - 1)) * clusterWidth / easy.groups;
	if (stackWidth < 0) {
		paddingGrouped = (1. - easy.groups / clusterWidth) / (easy.groups - 1);
		stackWidth = 0;
	}

	auto groupPadding = paddingGrouped * barWidth;

	// TODO: �˴����������barWidth����ΪstackPadding��y��ĳ߶ȣ�barWidth��x��ĳ߶�
	auto stackPadding = paddingStacked_ * barWidth; 

	struct KpVtxBuffer_
	{
		point3f pos;
		point4f clr;
	};

	auto disc = discreted_();
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
				auto paddedBottom = bottom + stackPadding * KuMath::sign(bottom - baseLine_);

				// ��һ������ȡright-top�������ɱ�֤���һ������Ϊleft-top��quad�����㰴˳ʱ�����У�
				// ���ȷ����flatģʽ����ʾleft-top�������ɫ����֤��y���ֵʱ����ȷ�ԣ�
				vtx[0].pos = point3f(right, top, pt[2]);
				pt[0] = right, pt[1] = top;
				vtx[0].clr = mapValueToColor_(pt.data(), ch);

				vtx[1].pos = point3f(right, paddedBottom, pt[2]);
				pt[1] = bottom;
				vtx[1].clr = mapValueToColor_(pt.data(), ch);

				vtx[2].pos = point3f(left, paddedBottom, pt[2]);
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

	return paint->drawGeomColor(geom);
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
	unsigned ydim = 1; // ��Ϊy�������ά��
	if (odata()->dim() > 1) {
		ydim = 2; // 1ά����Ϊ��������ѵ���
		if (odata()->dim() > 2 && odata()->channels() == 1) // 1/2ά�ȷֱ���Ϊ����/�ѵ�����ѵ�/������
			ydim = 3;
	}

	return ydim;
}


KcBars2d::aabb_t KcBars2d::calcBoundingBox_() const
{
	auto aabb = super_::calcBoundingBox_();

	if (!empty()) {
		auto w = barWidth_();
		aabb.inflate(w / 2, 0);

		auto easy = easyGetter_();
		
		unsigned ydim = ydim_();
		if (ydim != 1) {
			auto r = odata()->range(ydim);
			aabb.lower().y() = r.low();
			aabb.upper().y() = r.high();
		}

		for (unsigned c = 0; c < easy.clusters; c++) {
			for (unsigned g = 0; g < easy.groups; g++) {

				// ����1���ѵ��ĳ���
				float_t stackLen(0);
				for (unsigned s = 0; s < easy.stacks; s++) {
					auto pt = easy.getter(c, g, s);
					stackLen += pt[ydim];
				}

				KuMath::uniteRange(aabb.lower().y(), aabb.upper().y(),
					stackLen + baseLine_, stackLen + baseLine_);
			}
		}

		KuMath::updateRange<float_t>(aabb.lower().y(), aabb.upper().y(), baseLine_); // ȷ����ʾbaseline
	}

	return aabb;
}


unsigned KcBars2d::majorColorsNeeded() const
{
	return coloringMode() == k_colorbar_gradiant ? -1
		: (odata() == nullptr ? 1 : // ����δ֪������1
			odata()->channels() > 1 ? odata()->channels() // ��ͨ�����ݣ�����ͨ����
			: (odata()->dim() == 1 ? 1 : discreted_()->size(1))); // ��ά��ͨ�����ݣ�����size(1)
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

	auto chs = odata()->channels();

	auto disc = discreted_();

	// NB: �������ַ���/�ѵ���ʽ����ʼ�ձ���0ά��������x�ᣬ����Ŀ����0ά�ȵ�������
	getter.clusters = disc->size(0);

	// �Ǿ��Ȳ�������ֻ�ܰ�ͨ�����飬���ܶѵ����޷���֤x����һ�£�
	if (!disc->isSampled()) {
		getter.stacks = 1;
		getter.groups = chs;

		if (odata()->dim() == 1) {
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

	// TODO: disc������KcMonoDiscreted����ʱ�޷�ת��Ϊsamp����ʵ�ֵ�����KcMonoSampled
	auto samp = std::dynamic_pointer_cast<const KvSampled>(disc);

	if (chs > 1) { // ���Ȱ�ͨ������/�ѵ�
		if (stackedFirst_) {
			getter.stacks = chs;
			getter.groups = 1;
			if (disc->dim() == 1 || samp == nullptr) {
				getter.getter = [this, disc](unsigned idx, unsigned group, unsigned stack) {
					auto pt = disc->pointAt(idx, stack);
					if (pt.size() < 3)
					    pt.push_back(defaultZ(stack));
					return pt;
				};
			}
			else {
				getter.groups = samp->size(1);
				getter.getter = [samp](unsigned idx, unsigned group, unsigned stack) {
					return samp->point(idx, group, stack);
				};
			}
		}
		else {
			getter.groups = chs;
			getter.stacks = 1;		
			if (disc->dim() == 1 || samp == nullptr) {
				getter.getter = [this, disc](unsigned idx, unsigned group, unsigned stack) {
					auto pt = disc->pointAt(idx, group);
					if (pt.size() < 3)
					    pt.push_back(defaultZ(group));
					return pt;
				};
			}
			else {
				getter.stacks = samp->size(1);
				getter.getter = [samp](unsigned idx, unsigned group, unsigned stack) {
					return samp->point(idx, stack, group);
				};
			}
		}
	}
	else if (disc->dim() == 1 || samp == nullptr) { // ��ͨ����ά�ȣ��������Ͷѵ�����Ϊ1
		getter.stacks = 1;
		getter.groups = 1;
		getter.getter = [this, disc](unsigned idx, unsigned group, unsigned stack) {
			assert(stack == 0 && group == 0);
			auto pt = disc->pointAt(idx, 0);
			if (pt.size() < 3)
			    pt.push_back(defaultZ(0));
			return pt;

		};
	}
	else { // ��ͨ����ά�ȣ�����ά�ȷ���/�ѵ�
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
					kIndex idxes[3] = { kIndex(idx), kIndex(stack), kIndex(group) };
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
					kIndex idxes[3] = { kIndex(idx), kIndex(group), kIndex(stack) };
					return samp->point(idxes, 0);
				};
			}
		}
	}

	return getter;
}
