#include <assert.h>
#include <vector>
#include <cstdarg>
#include "KuWindowFactory.h"
#include "KtuMath.h"
#include "KtuBitwise.h"
#include "KvData.h"
#include "functions.h"
#include "kDsp.h"


namespace kPrivate
{
	template<typename OP>
	class KtWindowImpl_ : public KvData
	{
	public:
		KtWindowImpl_(OP op) : op_(op) {}

		kIndex dim() const override {
			return 1;
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
			return 0;
		}

		kReal value(kIndex idx[], kIndex channel) const override {
			return 0; // TODO:
		}

		std::vector<kReal> point(kIndex idx[], kIndex channel) const override {
			return { 0 }; // TODO
		}

		kReal value(kReal pt[], kIndex channel) const override {
			return op_(pt[0]);
		}

	private:
		OP op_;
	};


	template<typename OP>
	std::shared_ptr<KvData> make_window(OP op) {
		return std::make_shared<KtWindowImpl_<OP>>(op);
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
