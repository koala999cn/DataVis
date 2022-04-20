#include "KgWindowing.h"
#include <cstdarg>
#include "KcSampled1d.h"
#include "KvContinued.h"


KgWindowing::KgWindowing(int type, kIndex nx, ...)
    : KgMultiply(nx, 1)
{
	std::va_list args;
	va_start(args, type);
	auto win = KuWindowFactory::create(type, args);
	va_end(args);

	auto data = KgMultiply::operator->();
	KtSampling<kReal> samp;
	samp.resetn(nx, win->range(0).low(), win->range(0).high(), 0.5f);

	for (kIndex i = 0; i < nx; i++) {
		auto x = samp.indexToX(i);
		*data->row(i) = win->value(x, 0);
	}
}
