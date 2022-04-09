#pragma once
#include "KtSampledArray.h"


class KcSampled2d : public KtSampledArray<2>
{
public:
    using super_ = KtSampledArray<2>;

    void resize(kIndex nx, kIndex ny, kIndex channels) {
        kIndex idx[2] = { nx, ny };
        super_::resize(idx, channels);
    }

    void resize(kIndex nx, kIndex ny) {
        resize(nx, ny, channels());
    }

    value_type value(kIndex x, kIndex y, kIndex channel) const {
        kIndex idx[2] = { x, y };
        return super_::value(idx, channel);
    }

/*
    void resize(kIndex nx, kIndex ny, kIndex channels) override;

    void resize(const KvData2d& data) override;

    kPoint3d value(kIndex ix, kIndex iy, kIndex channel = 0) const override;

    void setChannel(kIndex row, const kReal* data, kIndex channel);*/
};

