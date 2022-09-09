#include "KvTicker.h"
#include <regex>


KvTicker::KvTicker()
{
	ticks_ = 5;
	format_ = "%d";
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

