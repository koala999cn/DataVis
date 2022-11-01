#pragma once
#include "KtSampledArray.h"


class KcSampled1d : public KtSampledArray<1>
{
public:
    using super_ = KtSampledArray<1>;

    using super_::super_;

    KcSampled1d(kReal dx, kIndex channels = 1) {
        resize(0, channels);
        reset(0, 0, dx);
    }

    void resize(kIndex nx, kIndex channels) {
        super_::resize(&nx, channels);
    }

    void resize(kIndex nx) {
        super_::resize(&nx, 0);
    }

    auto sampleRate() const { 
        return super_::sampleRate(0);
    }

    auto nyquistRate() const { 
        super_::nyquistRate(0); 
    }

};

