#include "KcHeatMap.h"
#include "KvDiscreted.h"
#include "KuStrUtil.h"
#include "KvPaint.h"


KcHeatMap::KcHeatMap(const std::string_view& name)
	: super_(name)
{
	forceDefaultZ() = true;
	flatShading() = true;
	setColoringMode(k_colorbar_gradiant);
}


KcHeatMap::aabb_t KcHeatMap::boundingBox() const
{
	auto aabb = super_::boundingBox();

	if (!empty() && data()->dim() > 1) {

		float_t dx, dy;

		if (data()->isDiscreted()) {
			auto disc = std::dynamic_pointer_cast<KvDiscreted>(data());
			assert(disc->size() != 0);
			dx = disc->step(0);
			dy = disc->step(1);
		}
		else {
			dx = sampCount(0) == 0 ? 0 : data()->range(0).length() / sampCount(0);
			dy = sampCount(1) == 0 ? 0 : data()->range(1).length() / sampCount(1);
		}

		// aabb膨胀(dx/2, dy/2)
		aabb.inflate(dx/2, dy/2);
	}

	return aabb;
}


void KcHeatMap::drawImpl_(KvPaint* paint, point_getter2 getter, unsigned nx, unsigned ny, unsigned ch) const
{
	auto disc = std::dynamic_pointer_cast<KvDiscreted>(data());
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
		pt.x() += xshift, pt.y() += yshift;
		return pt;
	};

	super_::drawImpl_(paint, getterShift, nx + 1, ny + 1, ch);

/*	for (unsigned i = 0; i < disc->size(); i++) {
		auto pt = disc->pointAt(i, 0);
		paint->setColor(mapValueToColor_(pt.back(), ch));
		paint->fillRect({ pt[0] - half_dx, pt[1] - half_dy, 0 },
			{ pt[0] + half_dx, pt[1] + half_dy, 0 });
	}

	if (showBorder() && borderPen().visible()) {
		paint->apply(borderPen());
		for (unsigned i = 0; i < disc->size(); i++) {
			auto pt = disc->pointAt(i, 0);
			paint->drawRect({ pt[0] - half_dx, pt[1] - half_dy, 0 },
				{ pt[0] + half_dx, pt[1] + half_dy, 0 });
		}
	}*/

	if (showText_ && clrText_.a() != 0) {
		paint->setColor(clrText_);
		auto leng = paint->projectv({ dx, dy, 0 }).abs();
		for (unsigned ix = 0; ix < nx; ix++)
			for (unsigned iy = 0; iy < ny; iy++) {
				auto pt = getter(ix, iy);
				auto text = KuStrUtil::toString(pt.back()); // TODO: 获取正确的数据
				auto szText = paint->textSize(text.c_str());
				pt.z() = defaultZ(ch);
				if (szText.x() <= leng.x() && szText.y() <= leng.y())
					paint->drawText(pt, text.c_str(), 0); // 0代表pt为中心点
			}
	}
}
