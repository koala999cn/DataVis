#include "KvScaler.h"
#include <regex>


KvScaler::KvScaler()
{
	tickCount_ = 5;
	subtickCount_ = 4;
	format_ = "%g";
}


KvScaler::~KvScaler()
{

}


std::string KvScaler::genLabel_(double val) const
{
	char buf[64];
	sprintf_s(buf, format_.c_str(), val);

	// ½«"0.00000..."Ìæ»»Îª"0"
	std::regex re("^0[\\.,]?0*$");
	if (std::regex_match(buf, re))
		buf[1] = '\0';

	return buf;
}
