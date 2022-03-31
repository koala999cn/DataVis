#include <assert.h>
#include <vector>
#include <cstdarg>
#include "KuWindowFactory.h"
#include "KtuMath.h"
#include "KtuBitwise.h"
#include "KvData1d.h"
#include "functions.h"
#include "kDsp.h"


namespace kPrivate
{
	template<typename OP>
	class KtWindowImpl_ : public KvData1d
	{
	public:
		KtWindowImpl_(OP op) : op_(op) {}

		void reserve(kIndex nx, kIndex channels) override {}

		void resize(kIndex nx, kIndex channels) override {}

		kPoint2d value(kIndex idx, kIndex channel = 0) const override {
			kReal x = range(0).low() + idx * 0.001; // 默认采样频率1000
			return { x, y(x) };
		}

		kIndex count() const override { 
			return k_inf_count; 
		}

		kIndex channels() const override { return 1; }

		void clear() override {}

		bool empty() const override { return false; }

		kIndex length(kIndex axis) const override {
			return count();
		}

		kRange range(kIndex axis) const override {
			return { 0, 1 };
		}

		kReal step(kIndex axis) const override {
			return k_unknown_step;
		}

		kReal y(kReal x, kIndex channle = 0) const override {
			return op_(x);
		}

	private:
		OP op_;
	};


	template<typename OP>
	std::shared_ptr<KvData1d> make_window(OP op) {
		return std::make_shared<KtWindowImpl_<OP>>(op);
	}
}


std::shared_ptr<KvData1d> KuWindowFactory::create(int type, ...)
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
