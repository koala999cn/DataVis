#include "KvPlottable1d.h"
#include "KvDiscreted.h"


void KvPlottable1d::drawDiscreted_(KvPaint* paint, KvDiscreted* disc) const
{
	if (disc->dim() == 1)
		draw1d_(paint, disc);
	else if (disc->isSampled())
		draw2d_(paint, disc);
	else
		draw3d_(paint, disc);
}


void KvPlottable1d::draw1d_(KvPaint* paint, KvDiscreted* disc) const
{
	float_t z;

	unsigned ch(0);
	auto getter = [&disc, &ch, &z](unsigned i) {
		auto pt = disc->pointAt(i, ch);
		pt.push_back(z);
		return pt;
	};

	for (; ch < disc->channels(); ch++) {
		z = defaultZ(ch);
		drawImpl_(paint, getter, disc->size(), ch);
	}
}


void KvPlottable1d::draw2d_(KvPaint* paint, KvDiscreted* disc) const
{
	assert(disc->isSampled() && disc->dim() == 2);

	unsigned ch(0);
	kIndex row;
	auto getter = [&disc, &row, &ch](unsigned i) {
		auto n = row * disc->size(1) * disc->channels() + i;
		return disc->pointAt(n, ch);
	};
	
	// NB: ��draw3d_��֮ͬ�����ڣ�ÿ��ͨ����draw2d_�����л��ƣ��ɻ����ٲ�ͼ��
	// ��draw3d_��ÿ��ͨ��������Ϊһ��������л��ƣ�û�����и��
	for (; ch < disc->channels(); ch++) 
		for (row = 0; row < disc->size(0); row++)
			drawImpl_(paint, getter, disc->size(1), ch);
}


void KvPlottable1d::draw3d_(KvPaint* paint, KvDiscreted* disc) const
{
	unsigned ch(0);
	auto getter = [&disc, &ch](unsigned i) {
		return disc->pointAt(i, ch);
	};

	for (; ch < disc->channels(); ch++) 
		drawImpl_(paint, getter, disc->size(), ch);
}


typename KvPaint::point_getter1 KvPlottable1d::toPointGetter_(GETTER g, unsigned channel) const
{
	if (forceDefaultZ()) {
		auto z = defaultZ(channel);
		return [g, z](unsigned idx) {
			auto pt = g(idx);
			return point3(pt[0], pt[1], z);
		};
	}
	else {
		return [g](unsigned idx) {
			auto pt = g(idx);
			return point3(pt[0], pt[1], pt[2]);
		};
	}
}
