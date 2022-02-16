#pragma once
#include "kDsp.h"


class KuFilter
{
public:

    // 平滑阶梯函数, 用来生成0到1的平滑过渡值
    static kReal smoothstep(kReal a, kReal b, kReal x);


private:
    KuFilter() {}
    ~KuFilter() {}
};

