#pragma once
#include <array>
#include <blitz/array.h>
#include "KtSampled.h"
#include "base/KtuMath.h"


// 基于blitz++多维数组实现的采样数据
template<typename BASE, int DIM>
class KtSampledArray : public KtSampled<BASE, DIM>
{
public:
    using super_ = KtSampled<BASE, DIM>;

    KtSampledArray() = default;


    // 每个通道的数据总数
    kIndex count() const override {
        return channels() == 0 ? 0 : array_.size() / channels(); 
    }


    kIndex channels() const override {
        return array_.length(DIM);
    }

    void clear() override {
        array_.resize(0);
        super_::clear();
    }


    // 重载实现length，基类实现会有累计误差
    kIndex length(kIndex axis) const override {
        return array_.length(axis); 
    }

    value_type value(std::array<kIndex, DIM> idx, kIndex channel) const override {
        blitz::TinyVector<int, DIM + 1> index;
        for (unsigned i = 0; i < idx.size(); i++)
            index[i] = idx[i];
        index[DIM] = channel;
        return array_(index);
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


    // 更快速的实现
    kRange valueRange() const override {
        return KtuMath<value_type>::minmax(array_.dataFirst(), array_.size());
    }

    kReal* at(kIndex row) { return &array_(int(row)); }
    const kReal* at(kIndex row) const { return &array_(int(row)); }

    auto stride(kIndex axis) const { return array_.stride(axis); }


protected:
	blitz::Array<value_type, DIM+1> array_; // 增加1维度，以提供多通道支持
};
