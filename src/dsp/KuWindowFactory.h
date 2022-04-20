#pragma once
#include <memory>


//
// Windowing functions
//
// References:
//  [Kaiser:1980] James F. Kaiser and Ronald W. Schafer, "On
//      the Use of I0-Sinh Window for Spectrum Analysis,"
//      IEEE Transactions on Acoustics, Speech, and Signal
//      Processing, vol. ASSP-28, no. 1, pp. 105--107,
//      February, 1980.
//  [harris:1978] frederic j. harris, "On the Use of Windows for Harmonic
//      Analysis with the Discrete Fourier Transform," Proceedings of the
//      IEEE, vol. 66, no. 1, January, 1978.
//  [Nuttall:1981] Albert H. Nuttall, "Some Windows with Very Good Sidelobe
//      Behavior,"  IEEE Transactions on Acoustics, Speech, and Signal
//      Processing, vol. ASSP-29, no. 1, pp. 84-91, February, 1981.

class KvContinued;


class KuWindowFactory
{
public:
	KuWindowFactory() = delete;

	// prototypes
	enum KeType
	{
		k_hamming,          // Hamming
		k_hann,             // Hann
		k_povey,            // "povey" is a window KLADI made to be similar to Hamming but to go to zero at the
							// edges, it's pow((0.5 - 0.5*cos(n/N*2*pi)), 0.85)
		k_blackman,         // ceoff == 0.42
		k_blackmanharris,   // Blackman-harris (4-term)
		k_blackmanharris7,  // Blackman-harris (7-term)
		k_flattop,          // flat top (includes negative values)
		k_triangular,       // triangular
		k_rcostaper,        // raised-cosine taper (taper size unspecified)
		k_kaiser,           // Kaiser (beta factor unspecified)
		//k_kbd,              // Kaiser-Bessel derived window (beta factor unspecified)
	};

	// generic window object given type
	// _type   :   window type, e.g. LIQUID_WINDOW_KAISER
	static std::shared_ptr<KvContinued> create(int type, ...);
};
