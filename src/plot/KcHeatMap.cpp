#include "KcHeatMap.h"
#include "KvDiscreted.h"
#include "KuStrUtil.h"
#include "KvPaint.h"


KcHeatMap::KcHeatMap(const std::string_view& name)
	: super_(name)
{
	setForceDefaultZ(true); // 在3d空间显示热图平面
	setFlatShading(true);
	setColoringMode(k_colorbar_gradiant);
}


KcHeatMap::aabb_t KcHeatMap::calcBoundingBox_() const
{
	auto aabb = super_::calcBoundingBox_();

	if (!empty() && data()->dim() > 1) {

		auto disc = discreted_();
		assert(disc->size() != 0);
		auto dx = disc->step(0);
		auto dy = disc->step(1);

		// aabb膨胀(dx/2, dy/2)
		aabb.inflate(dx/2, dy/2);
	}

	return aabb;
}


void KcHeatMap::drawImpl_(KvPaint* paint, GETTER getter, unsigned nx, unsigned ny, unsigned ch) const
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

	// quad按照顺时针排列
	// opengl默认falt渲染模式使用最后一个顶点的数据
	// 综上，需要将顶点向右下方偏移半个步长（x正向，y负向）
	auto getterShift = [this, ny, getter, half_dx, half_dy](unsigned ix, unsigned iy) {
	
		auto xshift = half_dx;
		auto yshift = -half_dy;

		if (ix == 0) // 首列数据
			xshift = -xshift, ix++;

		if (iy == ny) // 首行数据
			yshift = -yshift, iy--;

		auto pt = getter(ix - 1, iy);
		pt[0] += xshift, pt[1] += yshift;
		return pt;
	};

	super_::drawImpl_(paint, getterShift, nx + 1, ny + 1, ch);

	if (showText_ && clrText_.a() != 0) {
		
		auto leng = paint->projectv({ dx, dy, 0 }).abs();
		auto minSize = paint->textSize("0");
		if (leng.x() < minSize.x() || leng.y() < minSize.y()) // 加1个总体判断，否则当nx*ny很大时，非常耗时
			return;

		paint->setColor(clrText_);
		for (unsigned ix = 0; ix < nx; ix++)
			for (unsigned iy = 0; iy < ny; iy++) {
				auto pt = getter(ix, iy);
				auto text = KuStrUtil::toString(pt[colorMappingDim()]);
				auto szText = paint->textSize(text.c_str());
				if (szText.x() <= leng.x() && szText.y() <= leng.y())
					paint->drawText(toPoint_(pt.data(), ch), text.c_str(), 
						KeAlignment::k_vcenter | KeAlignment::k_hcenter); 
			}
	}
}
