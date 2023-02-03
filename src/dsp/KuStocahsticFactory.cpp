#include "KuStocahsticFactory.h"
#include <string>
#include "bimap.h"
#include "KvExcitor.h"
#include "KuMath.h"
#include "KgRand.h"


namespace kPrivate
{
	enum KeStocahsticType
	{
		k_stocahstic_gauss,
		k_stocahstic_exponent,
		k_stocahstic_laplace,
		k_stocahstic_rayleigh,
		k_stocahstic_lognorm,
		k_stocahstic_cauchy,
		k_stocahstic_weibull,
		k_stocahstic_count
	};

	static std::pair<int, std::string> typeName[] = {
		{ k_stocahstic_gauss, "gauss" },
		{ k_stocahstic_exponent, "exponent" },
		{ k_stocahstic_laplace, "laplace" },
		{ k_stocahstic_rayleigh, "rayleigh" },
		{ k_stocahstic_lognorm, "lognorm" },
		{ k_stocahstic_cauchy, "cauchy" },
		{ k_stocahstic_weibull, "weibull" }
	};

	static auto typeMap = bimap(typeName);


	class KcGauss : public KvExcitor
	{
	public:

		KcGauss() : mu_(0), sigma_(1) {}

		KcGauss(kReal mu, kReal sigma) : mu_(mu), sigma_(sigma) {}

		kReal tick() override {
			return static_cast<kReal>(r_.gauss(mu_, sigma_));
		}

		std::pair<kReal, kReal> range() const override {
			return { mu_ - 6 * sigma_, mu_ + 6 * sigma_ };
		}

	private:
		KgRand r_;
		kReal mu_;
		kReal sigma_;
	};



	class KcExponent : public KvExcitor
	{
	public:

		KcExponent() : beta_(1) {}

		KcExponent(kReal beta) : beta_(beta) {}

		kReal tick() override {
			return static_cast<kReal>(r_.exponent(beta_));
		}

		std::pair<kReal, kReal> range() const override {
			return { 0, std::numeric_limits<kReal>::infinity() };
		}

	private:
		KgRand r_;
		kReal beta_;
	};


	class KcLaplace : public KvExcitor
	{
	public:

		KcLaplace() : beta_(1) {}

		KcLaplace(kReal beta) : beta_(beta) {}

		kReal tick() override {
			return static_cast<kReal>(r_.laplace(beta_));
		}

		std::pair<kReal, kReal> range() const override {
			return { -std::numeric_limits<kReal>::infinity(), std::numeric_limits<kReal>::infinity() };
		}

	private:
		KgRand r_;
		kReal beta_;
	};


	class KcRayleigh : public KvExcitor
	{
	public:

		KcRayleigh() : sigma_(1) {}

		KcRayleigh(kReal sigma) : sigma_(sigma) {}

		kReal tick() override {
			return static_cast<kReal>(r_.rayleigh(sigma_));
		}

		std::pair<kReal, kReal> range() const override {
			return { 0, std::numeric_limits<kReal>::infinity() };
		}

	private:
		KgRand r_;
		kReal sigma_;
	};


	class KcLognorm : public KvExcitor
	{
	public:

		KcLognorm() : mu_(0), sigma_(1) {}

		KcLognorm(kReal mu, kReal sigma) : mu_(mu), sigma_(sigma) {}

		kReal tick() override {
			return static_cast<kReal>(r_.lognorm(mu_, sigma_));
		}

		std::pair<kReal, kReal> range() const override {
			return { 0, std::numeric_limits<kReal>::infinity() };
		}

	private:
		KgRand r_;
		kReal mu_;
		kReal sigma_;
	};


	class KcCauchy : public KvExcitor
	{
	public:

		KcCauchy() : alpha_(0), beta_(1) {}

		KcCauchy(kReal alpha, kReal beta) : alpha_(alpha), beta_(beta) {}

		kReal tick() override {
			return static_cast<kReal>(r_.cauchy(alpha_, beta_));
		}

		std::pair<kReal, kReal> range() const override {
			return { -std::numeric_limits<kReal>::infinity(), std::numeric_limits<kReal>::infinity() };
		}

	private:
		KgRand r_;
		kReal alpha_;
		kReal beta_;
	};


	class KcWeibull : public KvExcitor
	{
	public:

		KcWeibull() : alpha_(1), beta_(1) {}

		KcWeibull(kReal alpha, kReal beta) : alpha_(alpha), beta_(beta) {}

		kReal tick() override {
			return static_cast<kReal>(r_.weibull(alpha_, beta_));
		}

		std::pair<kReal, kReal> range() const override {
			return { 0, std::numeric_limits<kReal>::infinity() };
		}

	private:
		KgRand r_;
		kReal alpha_;
		kReal beta_;
	};
}


unsigned KuStocahsticFactory::typeCount()
{
	return kPrivate::k_stocahstic_count;
}


const char* KuStocahsticFactory::typeName(unsigned typeIdx)
{
	return kPrivate::typeMap.hasX(typeIdx) ?
		kPrivate::typeMap.x2y(typeIdx).c_str() : "unknown";
}


KvExcitor* KuStocahsticFactory::create(const char* name)
{
	if (!kPrivate::typeMap.hasY(name))
		return nullptr;

	return create(kPrivate::typeMap.y2x(name));
}


KvExcitor* KuStocahsticFactory::create(unsigned typeIdx)
{
	using namespace kPrivate;

	switch (typeIdx)
	{
	case k_stocahstic_gauss:
		return new KcGauss;

	case k_stocahstic_exponent:
		return new KcExponent;

	case k_stocahstic_laplace:
		return new KcLaplace;

	case k_stocahstic_rayleigh:
		return new KcRayleigh;

	case k_stocahstic_lognorm:
		return new KcLognorm;

	case k_stocahstic_cauchy:
		return new KcCauchy;

	case k_stocahstic_weibull:
		return new KcWeibull;

	default:
		break;
	}

	return nullptr;
}
