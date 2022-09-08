#include "KcVlCoordSystem.h"
#include "KgAxis.h"
#include "KcVlPlot3d.h"
#include <vlGraphics/Applet.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include "KglPaint.h"


KcVlCoordSystem::KcVlCoordSystem()
	: KvCoordSystem(vec3(0, 0, 0), vec3(10, 10, 10))
{

}


void KcVlCoordSystem::setVisible(bool b)
{
	actorTree_->setEnabled(b);
}


bool KcVlCoordSystem::visible() const
{
	return actorTree_->isEnabled();
}


void KcVlCoordSystem::draw(KglPaint* paint) const
{
	if (visible()) {
		for (int i = 0; i < 12; i++) {
			auto ax = axis(i);
			if (ax && ax->visible()) {
				paint->setColor(ax->baselineColor());
				paint->SetLineWidth(ax->baselineSize());
				paint->drawLine(ax->start(), ax->end());
			}
		}
	}
}
