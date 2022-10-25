#include "KgWindowing.h"
#include <assert.h>
#include <cstdarg>
#include "KtuMath.h"
#include "windows.h"


KgWindowing::KgWindowing(unsigned frameSize, KeType type, ...)
	: win_(frameSize)
{
	std::va_list args;
	va_start(args, type);
	auto f = functor(type, args);
	va_end(args);

	KtuMath<double>::linspace(0, 1, 0, win_.data(), frameSize);
	for (unsigned i = 0; i < frameSize; i++)
		win_[i] = f(win_[i]);
}


void KgWindowing::porcess(double* x) const
{
	KtuMath<double>::mul(x, win_.data(), x, idim());
}


std::function<double(double)> KgWindowing::functor(KeType type, ...)
{
	switch (type)
	{
	case k_hamming:
		return hamming<double>();

	case k_hann:
		return hann<double>();

	case k_povey:
		return povey<double>();

	case k_blackman:
		return blackman<double>();

	case k_blackmanharris:
		return blackmanharris<double>();

	case k_blackmanharris7:
		return blackmanharris7<double>();

	case k_flattop:
		return flattop<double>();

	case k_triangular:
		return triangular<double>();

	case k_rectangle:
		return [](double) { return 1; };

	case k_rcostaper:
	{
		std::va_list args;
		va_start(args, type);
		auto w = rcostaper<double>(va_arg(args, double));
		va_end(args);
		return w;
	}

	case k_kaiser:
	{
		std::va_list args;
		va_start(args, type);
		auto w = kaiser<double>(va_arg(args, double));
		va_end(args);
		return w;
	}

	}

	return [](double) { return KtuMath<double>::nan; };
}


const char* KgWindowing::type2Str(KeType type)
{
	switch (type) {
	case k_hamming:			return "hamming";
	case k_hann:			return "hann";
	case k_povey:			return "povey";
	case k_blackman:		return "blackman";
	case k_blackmanharris:	return "blackmanharris";
	case k_blackmanharris7:	return "blackmanharris7";
	case k_flattop:			return "flattop";
	case k_triangular: 		return "triangular";
	case k_rectangle:		return "rectangle";
	case k_rcostaper:		return "rcostaper";
	case k_kaiser:			return "kaiser";
	default:				return "unknown";
	}
}


KgWindowing::KeType KgWindowing::str2Type(const char* str)
{
	if (0 == _stricmp(str, type2Str(k_hamming)))
		return k_hamming;

	if (0 == _stricmp(str, type2Str(k_hann)))
		return k_hann;

	if (0 == _stricmp(str, type2Str(k_povey)))
		return k_povey;

	if (0 == _stricmp(str, type2Str(k_blackman)))
		return k_blackman;

	if (0 == _stricmp(str, type2Str(k_blackmanharris)))
		return k_blackmanharris;

	if (0 == _stricmp(str, type2Str(k_blackmanharris7)))
		return k_blackmanharris7;

	if (0 == _stricmp(str, type2Str(k_flattop)))
		return k_flattop;

	if (0 == _stricmp(str, type2Str(k_triangular)))
		return k_triangular;

	if (0 == _stricmp(str, type2Str(k_rectangle)))
		return k_rectangle;

	if (0 == _stricmp(str, type2Str(k_rcostaper)))
		return k_rcostaper;

	if (0 == _stricmp(str, type2Str(k_kaiser)))
		return k_kaiser;

	return k_unknown;
}
