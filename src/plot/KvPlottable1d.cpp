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
	auto z = defaultZ();

	unsigned ch(0);
	auto getter = [&disc, &ch, &z](unsigned i) -> KvPaint::point3 {
		auto pt = disc->pointAt(i, ch);
		return { pt[0], pt[1], z };
	};

	for (; ch < disc->channels(); ch++) {
		drawImpl_(paint, getter, disc->size(), ch);
		z += stepZ();
	}
}


void KvPlottable1d::draw2d_(KvPaint* paint, KvDiscreted* disc) const
{
	assert(disc->isSampled() && disc->dim() == 2);

	unsigned ch(0);
	kIndex row;
	auto getter = [&disc, &row, &ch](unsigned i) -> KvPaint::point3 {
		auto n = row * disc->size(1) * disc->channels() + i;
		auto pt = disc->pointAt(n, ch);
		return { pt[0], pt[1], pt[2] };
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
	auto getter = [&disc, &ch](unsigned i) -> KvPaint::point3 {
		auto pt = disc->pointAt(i, ch);
		return { pt[0], pt[1], pt[2] };
	};

	for (; ch < disc->channels(); ch++) 
		drawImpl_(paint, getter, disc->size(), ch);
}
