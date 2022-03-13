#include "KvData1d.h"
#include <cmath>
#include <limits>


// TODO: 使用极值修正算法
kRange KvData1d::valueRange(kIndex channel) const 
{
    if (empty()) return { 0, 0 };

    kReal ymin = std::numeric_limits<kReal>::max();
    kReal ymax = std::numeric_limits<kReal>::lowest();

    for (kIndex i = 0; i < count(); i++) {
        auto y = value(i, channel).y;
        if (std::isnan<kReal>(y))
            continue;

        ymin = std::min(ymin, y);
        ymax = std::max(ymax, y);
    }

    return { ymin, ymax };
}