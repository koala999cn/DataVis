#pragma once
#include <array>
#include <blitz/array.h>
#include "KtSampled.h"
#include "base/KtuMath.h"


// 基于blitz++多维数组实现的采样数据
template<int DIM>
class KtSampledArray : public KtSampled<DIM>
{
public:
    using super_ = KtSampled<DIM>;

    KtSampledArray() = default;


    /// 重载基类接口

    // 每个通道的数据总数
    kIndex size() const override {
        return channels() == 0 ? 0 : array_.size() / channels(); 
    }

    // 重载实现size，基类实现会有累计误差
    kIndex size(kIndex axis) const override {
        return array_.length(axis); 
    }

    kIndex channels() const override {
        return array_.length(DIM);
    }

    void clear() override {
        array_.resize(0); super_::clear();
    }

    value_type value(kIndex idx[], kIndex channel) const override {
        return array_(makeTinyVector_(idx, channel));
    }

    // 更快速的实现
    kRange valueRange() const override {
        return KtuMath<value_type>::minmax(array_.dataFirst(), array_.size());
    }


    /// 有关写操作的成员方法

    void resize(kIndex shape[], kIndex channel) {
        super_::resize(shape);
        array_.resizeAndPreserve(makeTinyVector_(shape, channel));
    }

    void resize(kIndex shape[]) {
        resize(shape, channels());
    }

    // 重置通道数
    void resizeChannel(kIndex c) {
        auto extent = array_.extent();
        extent[DIM] = c;
        array_.resizeAndPreserve(extent);
    }

    // 增加1个数据通道
    void addChannel() {
        resizeChannel(channels() + 1);
    }

    kReal* data() { return array_.dataFirst(); }
    const kReal* data() const { return array_.dataFirst(); }


    kReal* row(kIndex idx) { return &array_(int(idx)); }
    const kReal* row(kIndex idx) const { return &array_(int(idx)); }

    auto stride(kIndex axis) const { return array_.stride(axis); }

    // 设置特定通道的数据
    // @idx: 大小等于DIM-1
    // @data: 设置的数据，长度等于size(DIM-1)
    void setChannel(kIndex idx[], kIndex channel, const kReal* data);


    // bps
    auto bytesPerSample() const { return sizeof(kReal) * channels(); }

    // N个采样点占据的内存大小(字节数)
    auto bytesOfSamples(kIndex N) const { return bytesPerSample() * N; }

    // 删除前rows行
    void popFront(kIndex rows);

    // 移除第0轴数值小于x的rows
    void cutBefore(kReal x);

private:
    static auto makeTinyVector_(kIndex idx[], kIndex channel) {
        blitz::TinyVector<int, DIM + 1> tv;
        std::copy(idx, idx + DIM, tv.begin());
        tv[DIM] = channel;
        return tv;
    }

private:
	blitz::Array<value_type, DIM+1> array_; // 增加1维度，以提供多通道支持
};


template<int DIM>
void KtSampledArray<DIM>::setChannel(kIndex idx[], kIndex channel, const kReal* data) {
    assert(channel >= 0 && channel < channels());

    blitz::TinyVector<int, DIM + 1> tv;
    std::copy(idx, idx + DIM - 1, tv.begin());
    tv[DIM - 1] = 0;
    tv[DIM] = channel;

    kReal* dst = &array_(tv);
    if (stride(DIM - 1) == 1) {
        std::copy(data, data + size(DIM - 1), dst);
    }
    else {
        dst += channel * stride(DIM);
        for (kIndex i = 0; i < size(DIM - 1); i++) {
            *dst = *data++;
            dst += stride(DIM - 1);
        }
    }
}


template<int DIM>
void KtSampledArray<DIM>::popFront(kIndex rows)
{
    std::copy(row(rows), array_.dataFirst() + array_.size(), array_.dataFirst());
    auto shape = array_.extent();
    shape[0] -= rows;
    array_.resizeAndPreserve(shape);
    samp_[0].cutHead(rows);
}


template<int DIM>
void KtSampledArray<DIM>::cutBefore(kReal x)
{
    if (x <= samp_[0].low())
        return;

    auto nx = std::min(samp_[0].size(x - samp_[0].low()), size(0));
/*
    if (nx <= 0) {
        assert(empty() || point(&x, 0)[0] >= x);
    }
    else {
        assert(point(nx - 1)[0] < x);
        if (nx < size(0))
            assert(point(nx)[0] >= x);
    }*/

    if(nx > 0)
        popFront(nx);
}