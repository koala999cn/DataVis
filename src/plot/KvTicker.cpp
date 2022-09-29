#include "KvTicker.h"
#include <regex>


KvTicker::KvTicker()
{
	ticks_ = 5;
	subticks_ = 4;
	format_ = "%g";
}


KvTicker::~KvTicker()
{

}


std::string KvTicker::label(double val) const
{
	char buf[64];
	sprintf_s(buf, format_.c_str(), val);

	// ½«"0.00000..."Ìæ»»Îª"0"
	std::regex re("^0[\\.,]?0*$");
	if (std::regex_match(buf, re))
		buf[1] = '\0';

	return buf;
}


std::vector<double> KvTicker::getSubticks(const std::vector<double>& ticks, unsigned subticks)
{
	if (subticks == 0)
		subticks = subticks_;

	std::vector<double> res;
	if (ticks.size() > 1) {
		res.reserve((ticks.size() - 1) * subticks);
		for (unsigned i = 1; i < ticks.size(); i++) {
			auto ts = getTicks(ticks[i], ticks[i - 1], subticks + 2);
			for (unsigned j = 1; j < ts.size() - 1; j++)
				res.push_back(ts[j]);
		}
	}

	return res;
}