#pragma once
#include "KvData.h"


class KvData2d : public KvData
{
public:

    /// 实现基类接口

    kIndex dim() const final { return 2; }


    /// 定义自身接口

    virtual void resize(kIndex nx, kIndex ny, kIndex channels) = 0;

    // 重置this的尺寸和采样参数与data一致
    virtual void resize(const KvData2d& data) = 0;

    // 预留size个数据点的空间
    //virtual void reserve(kIndex nx, kIndex ny, kIndex channels) = 0;
   

    // 获取channel通道的第idx个数据点的坐标值(x, y)
    struct kPoint3d { kReal x, y, z; };
    virtual kPoint3d value(kIndex ix, kIndex iy, kIndex channel = 0) const = 0;
};

