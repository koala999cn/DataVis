#pragma once
#include "KtSampledArray.h"


class KcSampled2d : public KtSampledArray<2>
{
public:
    using super_ = KtSampledArray<2>;

    using super_::super_;

    KcSampled2d(kReal dx, kReal dy, kIndex channels = 1) {
        resize(0, 0, channels);
        reset(0, 0, dx);
        reset(1, 0, dy);
    }

    void resize(kIndex nx, kIndex ny, kIndex channels) {
        kIndex idx[2] = { nx, ny };
        super_::resize(idx, channels);
    }

    void resize(kIndex nx, kIndex ny) {
        resize(nx, ny, channels());
    }

    using super_::value;
    kReal value(kIndex x, kIndex y, kIndex channel) const {
        kIndex idx[2] = { x, y };
        return value(idx, channel);
    }

};

