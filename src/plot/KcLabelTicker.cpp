#include "KcLabelTicker.h"
#include "KtSampling.h"


void KcLabelTicker::update(double lower, double upper, bool)
{
	auto curTicks = labels_.size();
	labels_.resize(ticksExpected());

	if (labels_.size() > curTicks) { //初始化新增的labels
		KtSampling<double> samp;
		samp.resetn(ticksExpected(), lower, upper, 0.5);
		for (unsigned i = curTicks; i < labels_.size(); i++) {
			auto val = samp.indexToX(i);
			auto label = std::to_string(val);
			labels_[i].first = val;
			labels_[i].second = label;
		}
	}
}


unsigned KcLabelTicker::ticksTotal() const
{
	return ticksExpected();
}


unsigned KcLabelTicker::subticksTotal() const
{
	return 0; // 无副刻度
}


double KcLabelTicker::tick(unsigned idx) const
{
	return labels_[idx].first;
}


double KcLabelTicker::subtick(unsigned idx) const
{
	return std::numeric_limits<double>::quiet_NaN();
}


std::string KcLabelTicker::label(unsigned idx) const
{
	return labels_[idx].second;
}


void KcLabelTicker::setTick(unsigned idx, double val)
{
	if (labels_.size() <= idx)
		labels_.resize(idx + 1);

	labels_[idx].first = val;
}


void KcLabelTicker::setLabel(unsigned idx, const std::string_view& label)
{
	if (labels_.size() <= idx)
		labels_.resize(idx + 1);

	labels_[idx].second = label;
}


void KcLabelTicker::setTickLabel(unsigned idx, double val, const std::string_view& label)
{
	if (labels_.size() <= idx)
		labels_.resize(idx + 1);

	labels_[idx].first = val;
	labels_[idx].second = label;
}
