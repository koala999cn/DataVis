#include "KvPlot2d.h"
#include "KvCoord.h"
#include "KvPaint.h"


KvPlot2d::KvPlot2d(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord)
    : KvPlot(paint, coord, 2)
{
    setMargins(15, 15, 15, 15);
}


void KvPlot2d::autoProject_()
{
    auto lower = paint().localToWorldP(coord().lower());
    auto upper = paint().localToWorldP(coord().upper());

    // 确保z值区域包含0，许多地方使用0为默认的z值坐标
    if (lower.z() > 0)
        lower.z() = 0;
    else if (upper.z() < 0)
        upper.z() = 0;

    if (upper.z() == lower.z())
        upper.z() = lower.z() + 1; // 防止z轴尺度为0，否则构建透视矩阵含有nan值

    auto proj = KtMatrix4<float_t>::projectOrtho(lower, upper);

    // 调整z轴位置，给near/far平面留出足够空间
    // NB: !!! VERY IMPORTANT !!!
    proj = proj * mat4::buildTanslation({ 0, 0, lower.z() - 1 });

    setProjMatrix(proj);
}
