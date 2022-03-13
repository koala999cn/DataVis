#pragma once
#include "KtSampledN.h"
#include "KvData2d.h"


class KcSampled2d : public KtSampledN<KvData2d, 2>
{
public:
	//KcSampled2d();
	//virtual ~KcSampled2d();


    void resize(kIndex nx, kIndex ny, kIndex channels) override;

    kPoint3d value(kIndex ix, kIndex iy, kIndex channel = 0) const override;
};

