#include "KvData1d.h"
#include "KtuMath.h"


// TODO: 使用极值修正算法
kRange KvData1d::yrange(kIndex channel) const 
{
    kIndex i = 0;
    kReal ymin = value(0, channel).second;
    while (std::isnan<kReal>(ymin) && i < count() - 1)
        ymin = value(++i, channel).second; // 搜索定位第一个有效数据

    kReal ymax(ymin);

    for (i; i < count(); i++) {
        auto y = value(i, channel).second;
        if (std::isnan<kReal>(y))
            continue;

        ymin = std::min(ymin, y);
        ymax = std::max(ymax, y);
    }

    return { ymin, ymax };
}