#pragma once
#include <string>
#include <memory>
#include "KvData.h"
#include "kDsp.h"
#include "KtSampled.h"

class KvExprtk;

class KcFormulaData1d : public KtSampled<1>
{
public:
    KcFormulaData1d(kReal xmin, kReal xmax, kIndex nx, const std::shared_ptr<KvExprtk>& expr);


    kIndex channels() const override {
        return 1; // 只支持单通道
    }

    kReal value(kIndex[], kIndex) const override;

    kReal value(kReal pt[], kIndex channel) const override;


private:
    std::shared_ptr<KvExprtk> expr_;
};


