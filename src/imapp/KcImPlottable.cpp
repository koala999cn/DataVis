#include "KcImPlottable.h"
#include "implot/implot.h"
#include "KvDiscreted.h"


namespace kPrivate
{
	struct KpGetterHelper_
	{
		KvDiscreted* disc;
		kIndex channel;
	};

	static ImPlotPoint discGetter(int i, void* data) {
		KpGetterHelper_* helper = (KpGetterHelper_*)data;
		auto pt = helper->disc->pointAt(i, helper->channel);
		return ImPlotPoint(pt[0], pt[1]);
	}
}


void KcImPlottable::draw(KvPaint*) const
{
	using namespace kPrivate;

	auto d = data();
	if (d && d->isDiscreted()) {
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(d);
		KpGetterHelper_ helper;
		helper.disc = disc.get();
		for (kIndex ch = 0; ch < disc->channels(); ch++) {
			helper.channel = ch;
			ImPlot::PlotLineG(name().c_str(), kPrivate::discGetter, &helper, disc->size());
		}
	}
}
