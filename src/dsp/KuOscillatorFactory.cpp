#include "KuOscillatorFactory.h"
#include <string>
#include "bimap.h"
#include "KvOscillator.h"
#include "KtuMath.h"


namespace kPrivate
{
	enum KeOscillatorType
	{
		k_oscillator_sine,
		k_oscillator_triangle,
		k_oscillator_saw,
		k_oscillator_square,
		k_oscillator_pulse,
		k_oscillator_count
	};

	static std::pair<int, std::string> typeName[] = {
		{ k_oscillator_sine, "sine" },
		{ k_oscillator_triangle, "triangle" },
		{ k_oscillator_saw, "saw" },
		{ k_oscillator_square, "square" },
		{ k_oscillator_pulse, "pulse" }	
	};

	static auto typeMap = bimap(typeName);

	class KcSine : public KvOscillator
	{
	public:
		KcSine() : KvOscillator(2 * KtuMath<kReal>::pi) {}

		kReal tickImpl_(kReal phase) const final {
			return sin(phase);
		}
	};

	class KcTriangle : public KvOscillator
	{
	public:
		KcTriangle() : KvOscillator(1) {}

		kReal tickImpl_(kReal phase) const final {
			kReal tmp = phase <= 0.5 ? phase * 2 : (1 - phase) * 2;
			return tmp * 2 - 1;
		}
	};

	class KcSaw : public KvOscillator
	{
	public:
		KcSaw() : KvOscillator(1) {}

		kReal tickImpl_(kReal phase) const final {
			return phase * 2 - 1;
		}
	};

	class KcSquare : public KvOscillator
	{
	public:
		KcSquare() : KvOscillator(1) {}

		kReal tickImpl_(kReal phase) const final {
			return 	phase <= 0.5 ? -1 : 1;
		}
	};

	class KcPulse : public KvOscillator
	{
	public:
		KcPulse() : KvOscillator(1) {}

		kReal tickImpl_(kReal phase) const final {
			auto res = phase < prePhase_ ? 1 : 0;
			prePhase_ = phase;
			return res;
		}

		std::pair<kReal, kReal> range() const override {
			return { 0, 1 };
		}

	private:
		mutable kReal prePhase_{ 1 };
	};
}


unsigned KuOscillatorFactory::typeCount()
{
	return kPrivate::k_oscillator_count;
}


const char* KuOscillatorFactory::typeName(unsigned typeIdx)
{
	return kPrivate::typeMap.hasX(typeIdx) ?
		kPrivate::typeMap.x2y(typeIdx).c_str() : "unknown";
}


KvOscillator* KuOscillatorFactory::create(const char* name)
{
	if (!kPrivate::typeMap.hasY(name))
		return nullptr;

	return create(kPrivate::typeMap.y2x(name));
}


KvOscillator* KuOscillatorFactory::create(unsigned typeIdx)
{
	using namespace kPrivate;

	switch (typeIdx)
	{
	case k_oscillator_sine:
		return new KcSine;

	case k_oscillator_triangle:
		return new KcTriangle;

	case k_oscillator_saw:
		return new KcSaw;

	case k_oscillator_square:
		return new KcSquare;

	case k_oscillator_pulse:
		return new KcPulse;

	default:
		break;
	}
	
	return nullptr;
}
