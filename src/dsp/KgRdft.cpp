#include "KgRdft.h"
#include "../../3rdparty/praat/NUM2.h"
#include "../../3rdparty/praat/NUMfft_d.inl"


#define PRAAT_TABLE ((autoNUMfft_Table*)internalTable_)


KgRdft::KgRdft(unsigned N, bool kaldiCompat) ：size_(N), kaldiCompat_(kaldiCompat)
{
    table_ = new autoNUMfft_Table;
    ::NUMfft_Table_init(PRAAT_TABLE, N);
}


KgRdft::~KgRdft()
{
    delete PRAAT_TABLE;
}


void KgRdft::forward(kReal data[]) const
{
    ::NUMfft_forward(PRAAT_TABLE, data);

    if(kaldiCompat_) { // 调整结果布局，以兼容NR fft
        auto n = PRAAT_TABLE->n;
        if (n > 1) {
            // To be compatible with old behaviour
            kReal tmp = data[n - 1];
            for (long i = n - 1; i > 1; i--) {
                data[i] = data[i - 1];
            }
            data[1] = tmp;
        }
    }
}

void KgRdft::backward(kReal data[]) const
{
    if(kaldiCompat_) {
        auto n = PRAAT_TABLE->n;
        if (n > 1) {
            // To be compatible with old behaviour
            kReal tmp = data[1];
            for (long i = 1; i < n - 1; i++) {
                data[i] = data[i + 1];
            }
            data[n - 1] = tmp;
        }
    }

    ::NUMfft_backward(PRAAT_TABLE, data);
}

void KgRdft::computePowerSpectrum(const kReal *fft/*in*/, kReal* spec/*out*/) const
{
    // now we have in waveform, first half of complex spectrum
    // it's stored as [real0, realN/2-1, real1, im1, real2, im2, ...]

    auto half_dim = size_ / 2;

    spec[0] = fft[0] * fft[0]; // first_energy

    if(!kaldiCompat_) {
        for (decltype (half_dim) i = 1; i < half_dim; i++)
        {
            kReal real = fft[i * 2 - 1], im = fft[i * 2];
            spec[i] = real*real + im*im;
        }

        // last_energy, handle this special case
        // Will actually never be used, and anyway
        // if the signal has been bandlimited sensibly this should be zero.
        spec[half_dim] = fft[size_ - 1] * fft[size_ - 1];

    }
    else {
        for (decltype (half_dim) i = 1; i < half_dim; i++)
        {
            kReal real = fft[i * 2], im = fft[i * 2 + 1];
            spec[i] = real*real + im*im;
        }

        spec[half_dim] = fft[1] * fft[1];  // handle this special case
    }
}

