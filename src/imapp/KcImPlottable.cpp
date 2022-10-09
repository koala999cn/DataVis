#include "KcImPlottable.h"
#include "implot/implot.h"
#include "KvDiscreted.h"


namespace kPrivate
{
	static ImPlotPoint discGetter(int i, void* data) {
		KvDiscreted* disc = (KvDiscreted*)data;
		auto pt = disc->pointAt(i, 0);
		return ImPlotPoint(pt[0], pt[1]);
	}
}


void KcImPlottable::draw(KvPaint*) const
{
	auto d = data();
	if (d->isDiscreted()) {
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(d);
		ImPlot::PlotLineG(name().c_str(), kPrivate::discGetter, disc.get(), disc->size());
	}
}
