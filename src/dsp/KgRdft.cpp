#include "KgRdft.h"

#define FFT_DATA_TYPE double
#include "praat/NUM2.h"
#include "KtuMath.h"
#include <assert.h>


#define PRAAT_TABLE ((autoNUMfft_Table*)internalTable_)


KgRdft::KgRdft(unsigned sizeT, bool compatNR, bool normalize)
    : sizeT_(sizeT)
    , compatNR_(compatNR)
    , normalize_(normalize)
{
    internalTable_ = new autoNUMfft_Table;
    ::NUMfft_Table_init(PRAAT_TABLE, sizeT);
    assert(PRAAT_TABLE->n == sizeT);
}


KgRdft::~KgRdft()
{
    delete PRAAT_TABLE;
}


void KgRdft::forward(double data[]) const
{
    ::NUMfft_forward(PRAAT_TABLE, data);

    if(compatNR_) { // 调整结果布局，以兼容NR fft
        if (idim() > 1) {
            // To be compatible with old behaviour
            double tmp = data[idim() - 1];
            for (decltype(idim()) i = idim() - 1; i > 1; i--)
                data[i] = data[i - 1];

            data[1] = tmp;
        }
    }
}

void KgRdft::backward(double data[]) const
{
    if(compatNR_) {
        if (idim() > 1) {
            // To be compatible with old behaviour
            double tmp = data[1];
            for (decltype(idim()) i = 1; i < idim() - 1; i++) 
                data[i] = data[i + 1];

            data[idim() - 1] = tmp;
        }
    }

    ::NUMfft_backward(PRAAT_TABLE, data);

    if (normalize_) // data[i] /= N
        KtuMath<double>::scale(data, idim(), static_cast<double>(1.0 / idim()));
}

std::pair<double, double> KgRdft::unpack(const double* fft, unsigned idx) const
{
    assert(idx < odim());

    if (idx == 0)
        return { fft[0], 0 };
    else if (idx == idim() / 2) {
        if (!compatNR_) return { fft[idim() - 1], 0 };
        else return { fft[1], 0 };
    }

    if (!compatNR_)
        return { fft[idx * 2 - 1], fft[idx * 2] };
    else
        return { fft[idx * 2], fft[idx * 2 + 1] };
}

void KgRdft::powerSpectrum(const double *fft/*in*/, double* spec/*out*/) const
{
    // now we have in waveform, first half of complex spectrum
    // it's stored as [real0, realN/2-1, real1, im1, real2, im2, ...]

    auto half_dim = idim() / 2;

    spec[0] = fft[0] * fft[0]; // first_energy

    if(!compatNR_) {
        for (decltype(half_dim) i = 1; i < half_dim; i++) {
            double real = fft[i * 2 - 1], im = fft[i * 2];
            spec[i] = real * real + im * im;
        }

        // last_energy, handle this special case
        // Will actually never be used, and anyway
        // if the signal has been bandlimited sensibly this should be zero.
        spec[half_dim] = fft[idim() - 1] * fft[idim() - 1];

    }
    else {
        for (decltype(half_dim) i = 1; i < half_dim; i++) {
            double real = fft[i * 2], im = fft[i * 2 + 1];
            spec[i] = real * real + im * im;
        }

        spec[half_dim] = fft[1] * fft[1];  // handle this special case
    }

    if (normalize_) {
        // KtuMath<double>::forEach(spec, sizeF(), [](double x) { return std::sqrt(x); });
        // KtuMath<double>::scale(spec, sizeF(), static_cast<double>(2.0 / idim()));
        // spec[0] /= 2;
        KtuMath<double>::scale(spec, odim(), static_cast<double>(4.0 / idim() / idim()));
        spec[0] /= 4;
    }
}

