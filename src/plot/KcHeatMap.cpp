#include "KcHeatMap.h"
#include "KvSampled.h"
#include "KvPaint.h"
#include "KuStrUtil.h" // std::to_string��֧�ָ�ʽ����ʹ��KuStrUtil��ʽ��������


KcHeatMap::KcHeatMap(const std::string_view& name)
	: super_(name)
{
	setForceDefaultZ(true); // ��3d�ռ���ʾ��ͼƽ��
	setFlatShading(true);
	setColoringMode(k_colorbar_gradiant);
}


unsigned KcHeatMap::objectCount() const
{
	return odata()->channels() * 2;
}


bool KcHeatMap::objectVisible_(unsigned objIdx) const
{
	if (objIdx & 1)
		return showText_ && clrText_.a() != 0;
	else
		return super_::objectVisible_(objIdx);
}


void KcHeatMap::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	if (objIdx & 1)
		paint->setColor(clrText_);
	else
		super_::setObjectState_(paint, objIdx / 2);
}


KcHeatMap::aabb_t KcHeatMap::calcBoundingBox_() const
{
	auto aabb = super_::calcBoundingBox_();

	if (!empty() && odata()->dim() > 1) {

		auto disc = discreted_();
		assert(disc->size() != 0);
		auto dx = disc->step(0);
		auto dy = disc->step(1);

		// aabb����(dx/2, dy/2)
		aabb.inflate(dx/2, dy/2);
	}

	return aabb;
}


void* KcHeatMap::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	auto samp = std::dynamic_pointer_cast<const KvSampled>(discreted_());
	assert(samp && samp->dim() >= 2);

	auto ch = objIdx / 2;
	auto getter = [&samp, ch](unsigned ix, unsigned iy) {
		return samp->point(ix, iy, ch);
	};

	if (objIdx & 1) 
		return drawText_(paint, getter, samp->size(0), samp->size(1), ch);
	else 
		return drawImpl_(paint, getter, samp->size(0), samp->size(1), ch);
}


void* KcHeatMap::drawImpl_(KvPaint* paint, GETTER getter, unsigned nx, unsigned ny, unsigned ch) const
{
	auto disc = discreted_();
	auto dx = disc->step(0);
	auto dy = disc->dim() > 1 ? disc->step(1) : 0;
	if (dx <= 0)
		dx = disc->range(0).length() / disc->size(0);
	if (dy <= 0)
		dy = disc->range(1).length() / disc->size(disc->dim() > 1 ? 1 : 0);

	auto half_dx = dx / 2;
	auto half_dy = dy / 2;

	// quad����˳ʱ������
	// openglĬ��falt��Ⱦģʽʹ�����һ�����������
	// ���ϣ���Ҫ�����������·�ƫ�ư��������x����y����
	auto getterShift = [this, ny, getter, half_dx, half_dy](unsigned ix, unsigned iy) {
	
		auto xshift = half_dx;
		auto yshift = -half_dy;

		if (ix == 0) // ��������
			xshift = -xshift, ix++;

		if (iy == ny) // ��������
			yshift = -yshift, iy--;

		auto pt = getter(ix - 1, iy);
		pt[0] += xshift, pt[1] += yshift;
		return pt;
	};

	return super_::drawImpl_(paint, getterShift, nx + 1, ny + 1, ch);
}


void* KcHeatMap::drawText_(KvPaint* paint, GETTER getter, unsigned nx, unsigned ny, unsigned ch) const
{
	auto disc = discreted_();
	auto dx = disc->step(0);
	auto dy = disc->dim() > 1 ? disc->step(1) : 0;
	if (dx <= 0)
		dx = disc->range(0).length() / disc->size(0);
	if (dy <= 0)
		dy = disc->range(1).length() / disc->size(disc->dim() > 1 ? 1 : 0);

	auto leng = paint->projectv({ dx, dy, 0 }).abs();
	auto minSize = paint->textSize("0");
	if (leng.x() < minSize.x() || leng.y() < minSize.y()) // ��1�������жϣ�����nx*ny�ܴ�ʱ���ǳ���ʱ
		return nullptr;

	for (unsigned ix = 0; ix < nx; ix++) {
		for (unsigned iy = 0; iy < ny; iy++) {
			auto pt = getter(ix, iy);
			auto text = KuStrUtil::toString(pt[colorMappingDim()]);
			auto szText = paint->textSize(text.c_str());
			if (szText.x() <= leng.x() && szText.y() <= leng.y())
				paint->drawText(toPoint_(pt.data(), ch), text.c_str(),
					KeAlignment::k_vcenter | KeAlignment::k_hcenter);
		}
	}

	return nullptr;
}
