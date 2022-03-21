#pragma once
#include "KtSampledN.h"
#include "KvData2d.h"


class KcSampled2d : public KtSampledN<KvData2d, 2>
{
public:

    void resize(kIndex nx, kIndex ny, kIndex channels) override;

    void resize(const KvData2d& data) override;

    kPoint3d value(kIndex ix, kIndex iy, kIndex channel = 0) const override;

    void setChannel(kIndex row, kReal* data, kIndex channel);
};

