#pragma once
#include "KvCoordSystem.h"
#include <vlCore/Object.hpp>


class KcVlCoordSystem : public KvCoordSystem
{
public:

	KcVlCoordSystem();

	void draw(KglPaint*) const override;

private:
	void drawAxis_(KglPaint*, KgAxis*) const;
	void drawTicks_(KglPaint*, KgAxis*) const;
	void drawTick_(KglPaint*, KgAxis*, const vec3& pt, double length) const; // 绘制单条刻度线
};
