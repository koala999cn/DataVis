#pragma once
#include "kDsp.h"
#include "KvData.h"


using kPoint2d = std::pair<kReal, kReal>;

// 一维数据接口类
class KvData1d : public KvData
{
public:

    KvData1d() {}
    virtual ~KvData1d() {}

    /// 实现基类接口

    unsigned dim() final { return 1; }

    kRange range(int axis) final {
        return axis == 0 ? xrange() : yrange();
    }


    /// 定义自身接口

    // 获取数据点数目
    virtual kIndex count() const = 0;


    // 支持多通道数据，返回通道数目
    virtual kIndex channels() const = 0;


    // 预留size个数据点的空间
    virtual void reserve(kIndex size) = 0;


    // 获取channel通道的第idx个数据点的坐标值(x, y)
    virtual kPoint2d value(kIndex idx, kIndex channel = 0) const = 0;


    // 获取x值范围，用于设置横轴坐标
    virtual kRange xrange() const = 0;


    // 获取指定通道的y值范围，用于设置纵轴坐标. 提供暴力求解的缺省实现
    virtual kRange yrange(kIndex channel) const;


    // 获取所有通道数据的y值最小、最大值
    virtual kRange yrange() const {
        auto yr = yrange(0);
        for(kIndex i = 1; i < channels(); i++) {
            auto yri = yrange(i);
            yr.first = std::min(yr.first, yri.first);
            yr.second = std::max(yr.second, yri.second);
        }

        return yr;
    }
    
};
