#include "KcImPlottable1d.h"
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


void KcImPlottable1d::draw(KvPaint3d*) const
{
	using namespace kPrivate;

	auto d = data();
	if (d && d->isDiscreted()) {
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(d);
		KpGetterHelper_ helper;
		helper.disc = disc.get();
		for (kIndex ch = 0; ch < disc->channels(); ch++) {
			helper.channel = ch;

			switch (type_)
			{
			case k_line:
				ImPlot::PlotLineG(name().c_str(), kPrivate::discGetter, &helper, disc->size());
				break;

			case k_scatter:
				ImPlot::PlotScatterG(name().c_str(), kPrivate::discGetter, &helper, disc->size());
				break;

			case k_bars:
				ImPlot::PlotBarsG(name().c_str(), kPrivate::discGetter, &helper, disc->size(), 0.67);
				break;

			default:
				break;
			}
			
		}
	}
}
