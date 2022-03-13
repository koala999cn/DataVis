#pragma once
#include "kDsp.h"
#include "KvData.h"


// 一维数据接口类
class KvData1d : public KvData
{
public:

    /// 实现基类接口

    kIndex dim() const final { return 1; }


    /// 定义自身接口

    // 预留size个数据点的空间
    virtual void reserve(kIndex nx, kIndex channels) = 0;

    virtual void resize(kIndex nx, kIndex channels) = 0;

    // 获取channel通道的第idx个数据点的坐标值(x, y)
    struct kPoint2d { kReal x, y; };
    virtual kPoint2d value(kIndex idx, kIndex channel = 0) const = 0;

    /// helper funtions
    
    // 获取指定通道的y值范围，用于设置纵轴坐标. 提供暴力求解的缺省实现
    kRange valueRange(kIndex channel) const;

    // 获取所有通道数据的y值最小、最大值
    virtual kRange valueRange() const {
        auto yr = valueRange(0);
        for(kIndex i = 1; i < channels(); i++) {
            auto yri = valueRange(i);
            yr.resetLow(std::min(yr.low(), yri.low()));
            yr.resetHigh(std::max(yr.high(), yri.high()));
        }

        return yr;
    }
};
