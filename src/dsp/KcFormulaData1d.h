#pragma once
#include <string>
#include <memory>
#include "KvData1d.h"
#include "kDsp.h"
#include "KtSampling.h"

class KvExprtk;

class KcFormulaData1d : public KvData1d
{
public:
    KcFormulaData1d(kReal xmin, kReal xmax, kIndex nx, const std::shared_ptr<KvExprtk>& expr)
        : expr_(expr) { samp_.resetn(xmin, xmax, nx); }


    kIndex count() const override;

    kIndex channels() const override {
        return 1; // 暂时只支持单通道
    }

    kPoint2d value(kIndex idx, kIndex channel = 0) const override;

    kRange xrange() const override;


    /// 基本属性

    auto samplingRate() const { return samp_.rate(); }

    auto& sampling() const { return samp_; }

private:
    KtSampling<kReal> samp_;
    std::shared_ptr<KvExprtk> expr_;
};


