#pragma once
#include "KcLinearTicker.h"


class KcTimeTicker : public KcLinearTicker
{
public:

	std::string label(unsigned idx) const override;

	int format() const { return fmt_; }
	int& format() { return fmt_; }

	enum KeFormat
	{
		k_all_standard, // %c, writes standard date and time string, e.g.Sun Oct 17 04:41 : 13 2010 (locale dependent)
		k_all_alternative, // %Ec, writes alternative date and time string, e.g. using 平成23年 instead of 2011年 in ja_JP locale
		k_all_localized_date, // %x, writes localized date representation(locale dependent)
		k_all_alternative_date, // %Ex, writes alternative date representation, e.g. using 平成23年 instead of 2011年 in ja_JP locale
		k_all_localized_time, // %X, writes localized time representation, e.g. 18:40:20 or 6:40:20 PM(locale dependent)
		k_all_alternative_time, // %EX, writes alternative time representation(locale dependent)

		k_date_simple, // %D, equivalent to "%m/%d/%y"
		k_date_iso_8601, // %F, equivalent to "%Y-%m-%d"
		k_time_localized_12hour, // %r, writes localized 12-hour clock time(locale dependent)	
		k_time_simple, // %R, equivalent to "%H:%M"
		k_time_iso_8601, // %T, equivalent to "%H:%M:%S"
		k_count
	};

	unsigned formatCount() const { return k_count; }

	const char* formatText(int f) const;

	const char* const* formatTextList() const;

private:
	int fmt_{ 0 };
};
