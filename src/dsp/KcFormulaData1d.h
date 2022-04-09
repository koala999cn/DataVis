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
        : expr_(expr) { samp_.resetn(nx, xmin, xmax, 0); }


    kReal step(kIndex axis) const override;

    void clear() override;

    bool empty() const override;

    kIndex count() const override;

    kIndex length(kIndex axis) const override;

    kIndex channels() const override {
        return 1; // 暂时只支持单通道
    }

    //void reserve(kIndex nx, kIndex channel) override {}

    void resize(kIndex nx, kIndex channel)  {
        samp_.resetn(nx, samp_.dx(), samp_.x0ref());
    }


    kReal value(kIndex idx[], kIndex channel) const override;

    // 参数同上，不同的是返回数据数组，含有各坐标轴的数据值
    std::vector<kReal> point(kIndex idx[], kIndex channel) const override;

    // 通过坐标值获取数据值
    // @pt: 大小为dim，各元素分表表示对应坐标轴的坐标值
    kReal value(kReal pt[], kIndex channel) const override;


    kRange range(kIndex axis) const override;


    /// 基本属性

    auto samplingRate() const { return samp_.rate(); }

    auto& sampling() const { return samp_; }

private:
    KtSampling<kReal> samp_;
    std::shared_ptr<KvExprtk> expr_;
};


