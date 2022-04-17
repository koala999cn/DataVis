#include <assert.h>
#include <vector>
#include <cstdarg>
#include "KuWindowFactory.h"
#include "KtuMath.h"
#include "KtuBitwise.h"
#include "KtContinued.h"
#include "functions.h"
#include "kDsp.h"


namespace kPrivate
{
	template<typename OP>
	std::shared_ptr<KvData> make_window(OP op) {
		auto w = std::make_shared<KtContinued<OP, 1>>(op, 0, 1);
		return w;
	}
}


std::shared_ptr<KvData> KuWindowFactory::create(int type, ...)
{
	switch (type)
	{
	case k_hamming:
		return kPrivate::make_window(hamming<kReal>());
		
	case k_hann:
		return kPrivate::make_window(hann<kReal>());

	case k_povey:
		return kPrivate::make_window(povey<kReal>());

	case k_blackman:
		return kPrivate::make_window(blackman<kReal>());

	case k_blackmanharris:
		return kPrivate::make_window(blackmanharris<kReal>());

	case k_blackmanharris7:
		return kPrivate::make_window(blackmanharris7<kReal>());

	case k_flattop:
		return kPrivate::make_window(flattop<kReal>());

	case k_triangular:
		return kPrivate::make_window(triangular<kReal>());

	case k_rcostaper:
	{
		std::va_list args;
		va_start(args, type);
		auto w = kPrivate::make_window(rcostaper<kReal>(va_arg(args, kReal)));
		va_end(args);
		return w;
	}

	case k_kaiser:
	{
		std::va_list args;
		va_start(args, type);
		auto w = kPrivate::make_window(kaiser<kReal>(va_arg(args, kReal)));
		va_end(args);
		return w;
	}

	//case k_kbd:
	default:
		assert(false);
	}

	return 0;
}
