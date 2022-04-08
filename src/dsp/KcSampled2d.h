#pragma once
#include "KtSampledArray.h"
#include "KvData2d.h"


class KcSampled2d : public KtSampledArray<KvData2d, 2>
{
public:

    void resize(kIndex nx, kIndex ny, kIndex channels) override;

    void resize(const KvData2d& data) override;

    kPoint3d value(kIndex ix, kIndex iy, kIndex channel = 0) const override;

    void setChannel(kIndex row, const kReal* data, kIndex channel);
};

