#pragma once
#include "KvTicker.h"
#include <vector>


class KcLabelTicker : public KvTicker
{
public:

	void update(double lower, double upper, bool skipSubticks) override;

	unsigned ticksTotal() const override;

	unsigned subticksTotal() const override;

	double tick(unsigned idx) const override;

	double subtick(unsigned idx) const override;

	std::string label(unsigned idx) const override;

	void setTick(unsigned idx, double val);

	void setLabel(unsigned idx, const std::string_view& label);

	void setTickLabel(unsigned idx, double val, const std::string_view& label);

private:
	std::vector<std::pair<double, std::string>> labels_;
};
