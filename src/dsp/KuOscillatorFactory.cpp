#include "KuOscillatorFactory.h"
#include <string>
#include "bimap.h"


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


KvExcitor* KuOscillatorFactory::create(const char* name)
{
	if (!kPrivate::typeMap.hasY(name))
		return nullptr;

	return create(kPrivate::typeMap.y2x(name));
}


KvExcitor* KuOscillatorFactory::create(unsigned typeIdx)
{
	using namespace kPrivate;

	switch (typeIdx)
	{
	case k_oscillator_sine:
		break;

	case k_oscillator_triangle:
		break;

	case k_oscillator_saw:
		break;

	case k_oscillator_square:
		break;

	case k_oscillator_pulse:
		break;

	default:
		break;
	}

	return nullptr;
}
