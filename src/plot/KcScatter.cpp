#include "KcScatter.h"
#include "plot/KvPaint.h"
#include "KvData.h"


unsigned KcScatter::objectCount() const
{ 
	return empty() ? 0 : channels_();
} 


bool KcScatter::objectVisible_(unsigned objIdx) const
{
	return true;
}


void KcScatter::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	paint->apply(marker_);
	if (coloringMode() == k_one_color_solid)
		paint->setColor(majorColor(objIdx));
}


void* KcScatter::drawObject_(KvPaint* paint, unsigned objIdx) const
{
#if 0
	auto ch = objIdx / 2;

	if (objIdx == 1) {
		for (kIndex ch = 0; ch < odata()->channels(); ch++) {
			paint->setColor(majorColor(ch)); // �߶β�����
			for (unsigned i = 0; i < linesPerChannel_(); i++) {
				auto g = lineAt_(ch, i);
				paint->drawLineStrip(toPoint3Getter_(g.getter, ch), g.size);
			}
		}
	}
	else {
		for (kIndex ch = 0; ch < odata()->channels(); ch++) {
			for (unsigned i = 0; i < linesPerChannel_(); i++) {
				auto g = lineAt_(ch, i);
				for (unsigned i = 0; i < g.size; i++) {
					auto val = g.getter(i);
					paint->setColor(mapValueToColor_(val.data(), ch)); // ֧�ֽ���ɫ
					auto pt = toPoint_(val.data(), ch);
					paint->drawMarker({ pt[0], pt[1], pt[2] });
				}
			}
		}
	}
#endif

/*	for (kIndex ch = 0; ch < odata()->channels(); ch++) {
		for (unsigned i = 0; i < linesPerChannel_(); i++) {
			auto g = lineAt_(ch, i);
			for (unsigned i = 0; i < g.size; i++) {
				auto val = g.getter(i);
				paint->setColor(mapValueToColor_(val.data(), ch)); // ֧�ֽ���ɫ
				auto pt = toPoint_(val.data(), ch);
				paint->drawMarker({ pt[0], pt[1], pt[2] });
			}
		}
	}*/

	auto g = KuDataUtil::pointsAt(discreted_(), objIdx);

	if (coloringMode() == k_one_color_solid) {
		return paint->drawMarkers(toPoint3Getter_(g.getter, objIdx), g.size);
	}

	return nullptr; 
}


const color4f& KcScatter::minorColor() const
{
	return marker_.outline;
}


void KcScatter::setMinorColor_(const color4f& minor)
{
	marker_.outline = minor;
}
