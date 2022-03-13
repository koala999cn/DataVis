#pragma once
#include <array>
#include <blitz/array.h>
#include "KtSampling.h"
#include "base/KtuMath.h"
#include "KvData.h"


// 基于blitz++多维数组实现的多维采样
// @BASE: 须是KvData的子类
// @DIM: DIM=1时创建2维array, DIM=2时创建3维array, ...
template<typename BASE, int DIM>
class KtSampledN : public BASE
{
public:
    using value_type = kReal;

    /// 实现KvData的接口


    // 每个通道的数据总数
    kIndex count() const override {
        return channels() == 0 ? 0 : array_.size() / channels(); 
    }


    kIndex channels() const override {
        return array_.length(DIM);
    }

    void clear() override {
        array_.resize(0);
    }


    // 第axis维度的大小
    // assert(0 <= aixs < DIM)，下同
    kIndex length(kIndex axis) const override {
        return array_.length(axis); 
    }

    kRange range(kIndex axis) const override {
        return axis < DIM ? samp_[axis] : valueRange();
    }

    value_type step(kIndex axis) const override {
        return samp_[axis].dx(); 
    }


    // reshape the array
    // 目前blitz++只支持到11个维度的多参数实现，更多维度的可使用TinyVector
    //std::enable_if_t<sizeof...(Indices) <= std::min(DIM + 1, 11), void>
    //void resize(int... indices) {
    //    array_.resizeAndPreserve(indices...);
    //}

    // 重置通道数
    void resizeChannel(int c) {
        auto extent = array_.extent();
        extent[DIM] = c;
        array_.resizeAndPreserve(extent);
    }

    // 增加1个数据通道
    void addChannel() {
        resizeChannel(channels() + 1);
    }

 
    //at(int... indices) {
    ///    return array_(indices...);
    //}


    //at(int... indices) const {
    //    return array_(indices...);
    //}


    // 第channel通道的最大最小值
    kRange channelRange(int c) const {
        if (array_.size()) return { 0, 0 };

        value_type vmin = std::numeric_limits<value_type>::max();
        value_type vmax = std::numeric_limits<value_type>::lowest();

        auto p = array_.dataFirst() + c;
        for (int i = 0; i < count(); i++, p += array_.stride(DIM)) {
            auto val = *p;
            if (std::isnan<value_type>(val))
                continue;

            vmin = std::min(vmin, val);
            vmax = std::max(vmax, val);
        }

        return { vmin, vmax };
    }


    // 所有通道的最大最小值
    kRange valueRange() const {
        return KtuMath<value_type>::minmax(array_.dataFirst(), array_.size());
    }

/*
    value_type indexToValue(int axis, long idx) const {
        return samp_[axis].indexToX(idx);
    }


    long valueToIndex(int axis, value_type val) const {
        return samp_[axis].xToIndex(val);
    }
*/

protected:
	blitz::Array<value_type, DIM+1> array_; // 增加1维度，以提供多通道支持
	std::array<KtSampling<value_type>, DIM> samp_; // 各维度的采样参数，最内维度是数据，无采样参数
};
