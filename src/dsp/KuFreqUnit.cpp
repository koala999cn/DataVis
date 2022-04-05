#include "KuFreqUnit.h"
#include "KtuMath.h"
#include <assert.h>


double KuFreqUnit::melToHertz3(double mel)
{
	if (mel < 0) return KtuMath<double>::nan;
	return mel < 1000 ? mel : 1000 * (exp(mel * log10(2) / 1000) - 1);
}

double KuFreqUnit::hertzToMel3(double hz)
{
	if (hz < 0) return KtuMath<double>::nan;
	return hz < 1000 ? hz : 1000 * log10(1 + hz / 1000) / log10(2);
}

double KuFreqUnit::melToHertz2(double mel)
{
	if (mel < 0) return KtuMath<double>::nan;
	return 700 * (pow(10.0, mel / 2595.0) - 1);
}

double KuFreqUnit::hertzToMel2(double hz)
{
	if (hz < 0) return KtuMath<double>::nan;
	return 2595 * log10(1 + hz / 700);
}

double KuFreqUnit::hertzToBark1977(double hz)
{
	if (hz < 0) return KtuMath<double>::nan;
	return 7 * KtuMath<double>::asinh(hz / 650);
}

double KuFreqUnit::barkToHertz1977(double bark)
{
	return 650 * ::sinh(bark / 7);
}

double KuFreqUnit::hertzToBark1979_1(double hz)
{
	if (hz < 0) return KtuMath<double>::nan;
	return 13.3 * ::atan(0.75 * hz / 1000);
}

double KuFreqUnit::barkToHertz1979_1(double bark)
{
	return ::tan(bark / 13.3) * 1000 / 0.75;
}

double KuFreqUnit::hertzToBark1979_2(double hz)
{
	if (hz < 0) return KtuMath<double>::nan;
	return 12.82 * ::atan(0.78 * hz / 1000) + 0.17 * ::pow(hz / 1000, 1.4);
}

double KuFreqUnit::barkToHertz1979_2(double bark)
{
	assert(false && "not supported!");
	return KtuMath<double>::nan;
}

double KuFreqUnit::hertzToBark1980_1(double hz)
{
	if (hz < 0) return KtuMath<double>::nan;
	return 13 * ::atan(0.00076 * hz) + 3.5 * ::atan(hz * hz / 7500 / 7500);
}

double KuFreqUnit::barkToHertz1980_1(double bark)
{
	assert(false && "not supported!");
	return KtuMath<double>::nan;
}

double KuFreqUnit::hertzToBark1980_2(double hz)
{
	if (hz < 0) return KtuMath<double>::nan;
	return 8.7 + 14.2 * log10(hz / 1000);
}

double KuFreqUnit::barkToHertz1980_2(double bark)
{
	return ::pow(10, (bark - 8.7) / 14.2) * 1000;
}

double KuFreqUnit::hertzToBark1990(double hz)
{
	if (hz < 0) return KtuMath<double>::nan;
	return 26.81 * hz / (1960 + hz) - 0.53;
}

double KuFreqUnit::barkToHertz1990(double bark)
{
	if (bark < 0 || bark > 26.28) return KtuMath<double>::nan;
	return 1960 * (bark + 0.53) / (26.28 - bark);
}

double KuFreqUnit::hertzToBark1992(double hz)
{
	if (hz < 0) return KtuMath<double>::nan;
	return 6 * ::asinh(hz / 600);
}

double KuFreqUnit::barkToHertz1992(double bark)
{
	return ::sinh(bark / 6) * 600;
}

// TODO: 以下貌似与1980_1版本对应
double KuFreqUnit::criticalBand(double fc)
{
	return 25 + 75 * pow(1 + 1.4 * (fc / 1000) * (fc / 1000), 0.69);
}

double KuFreqUnit::erb1983(double fc)
{
	return 6.23e-6 * fc * fc + 0.09339 * fc + 28.52;
}

double KuFreqUnit::erb1990(double fc)
{
	return 24.673 * (0.004368 * fc + 1);
}

double KuFreqUnit::hertzToCam(double hertz)
{
	return hertz < 0 ? KtuMath<double>::nan : 21.366 * ::log10(0.004368 * hertz + 1);
}

double KuFreqUnit::camToHertz(double cam)
{
	return cam < 0 ? KtuMath<double>::nan : (::pow(10, cam / 21.366) - 1) / 0.004368;
}

double KuFreqUnit::hertzToCamPraat(double hertz)
{
	return hertz < 0 ? KtuMath<double>::nan : 11.17 * log((hertz + 312.0) / (hertz + 14680.0)) + 43.0;
}

double KuFreqUnit::camToHertzPraat(double cam)
{
	if (cam < 0) return KtuMath<double>::nan;

	double dum = exp((cam - 43.0) / 11.17);
	return  (14680.0 * dum - 312.0) / (1.0 - dum);
}

double KuFreqUnit::hertzToMel(double hertz)
{
	return hertzToMel2(hertz);
}

double KuFreqUnit::melToHertz(double mel)
{
	return melToHertz2(mel);
}

double KuFreqUnit::freqRatioToSemitones(double fr)
{
	if (fr <= 0.0) return KtuMath<double>::nan;

	return 12.0 * log(fr) / log(2.0);
}

double KuFreqUnit::semitonesToFreqRatio(double semitones)
{
	return exp(semitones * (log(2.0) / 12.0));
}

double KuFreqUnit::hertzToSemitones(double hertz, double base_hertz)
{
	return freqRatioToSemitones(hertz / base_hertz);
}

double KuFreqUnit::semitonesToHertz(double semitones, double base_hertz)
{
	return base_hertz * semitonesToFreqRatio(semitones);
}


double KuFreqUnit::hertzToBark1997(double hertz)
{
	double bark = hertzToBark1990(hertz);
	if (bark < 2.0)
		bark = bark + 0.15 * (2.0 - bark);
	else if (bark > 20.1)
		bark = bark + 0.22 * (bark - 20.1);
	return bark;
}


double KuFreqUnit::barkToHertz1997(double bark)
{
	if (bark < 2.0)
		bark = 2.0 + (bark - 2.0) / 0.85;
	else if (bark > 20.1)
		bark = 20.1 + (bark - 20.1) / 1.22;
	return barkToHertz1990(bark);
}