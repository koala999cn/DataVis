#pragma once
#include "KvData1d.h"
#include <vector>
#include "kDsp.h"
#include "KtSampling.h"


class KcSampled1d : public KvData1d
{
public:
    KcSampled1d() : channels_(1) {}
    KcSampled1d(const std::vector<kReal>& data, kIndex channel = 1);
    KcSampled1d(std::vector<kReal>&& data, kIndex channel = 1);

    KcSampled1d(const KcSampled1d& other);
    KcSampled1d(KcSampled1d&& other);

    // 给定采样率的空数据构造
    KcSampled1d(kReal dx, kReal x0_rel = 0, kIndex channel = 1);


    /// KvData接口实现

    kReal step(int axis) const override;

    // 清空数据, 重置采样区间(xmax = xmin)，但保持dx, d0基本设置
    void clear() override;


    /// KvData1d接口实现

    kIndex count() const override;

    kIndex channels() const override;

    void reserve(kIndex size) override;

    kPoint2d value(kIndex idx, kIndex channel = 0) const override;

    kRange xrange() const override;

    kRange yrange() const override;


    /// 基本属性

    auto samplingRate() const { return samp_.rate(); }

    auto& sampling() const { return samp_; }
    auto& sampling() { return samp_; }

    /// 数据写操作

    // 设第channel通道的第idx采样点的数据为val
    // 若channel < 0，则置idx采样点所有通道的数据都为val
    void setSample(kIndex idx, kReal val, kIndex channel);

    // 从第idx个采样点开始，用v重置N组采样点的数据
    void setSamples(kIndex idx, const kReal* v, kIndex N);

    // 从第idx个采样点开始，拷贝N组采样点的数据到v
    void getSamples(kIndex idx, kReal* v, kIndex N) const;

    // 增加N组采样点，第n采样点第i通道的值为v[n*channels()+i]
    void addSamples(kReal* v, kIndex N);

    // 按参数重置数据存储和采样参数
    void reset(kReal dx, kIndex channels, kIndex samples = 0);


    // 设置第channel通道的数据，确保data的大小等于this->count()
    // 如果channel=-1，则data包含所有通道数据
    void setChannel(kReal* data, kIndex channel);


    // bps
    auto bytesPerSample() const { return sizeof(kReal) * channels(); }

    // N个采样点占据的内存大小(字节数)
    auto bytesOfSamples(kIndex N) const { return bytesPerSample() * N; }


protected:

    // 计算第idx个采样点，第channel通道的数据偏移
    kIndex offset_(kIndex idx, kIndex channel = 0) const {
        return idx * channels() + channel;
    }


protected:
    KtSampling<kReal> samp_;
    std::vector<kReal> data_;
    kIndex channels_; // 通道数量
};

