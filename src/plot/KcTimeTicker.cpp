#include "KcTimeTicker.h"
#include <iomanip>
#include <sstream>


std::string KcTimeTicker::label(unsigned idx) const
{
	time_t sec = time_t(tick(idx));
	auto tm = std::gmtime(&sec);

	static const char* fmt[] = {
		"%c",
		"%Ec", 
		"%x",
		"%Ex", 
		"%X",
		"%EX",
		"%D",
		"%F",
		"%r",
		"%R", 
		"%T", 
	};

	std::ostringstream strm;
	strm << std::put_time(tm, fmt[fmt_]);

	return strm.str();
}


const char* const* KcTimeTicker::formatTextList() const
{
	static const char* text[] = {
		"standard data and time",
		"alternative data and time",
		"localized date and standard time",
		"alternative date and standard time",
		"standard data and localized time",
		"standard data and alternative time",

		"simple date(%m/%d/%y)",
		"iso 8601 date(%Y-%m-%d)",
		"localized 12hour time",
		"simple time(%H:%M)",
		"iso 8601 time(%H:%M:%S)"
	};

	return text;
}


const char* KcTimeTicker::formatText(int fmt) const
{
	return formatTextList()[fmt];
}