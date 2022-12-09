#include "KvPlot2d.h"
#include "KvCoord.h"


void KvPlot2d::autoProject_()
{
    auto box = coord().boundingBox();
    assert(box.width() != 0 && box.height() != 0);
    box.setExtent(2, 1); // 防止z轴尺度为0，否则构建透视矩阵含有nan值
    setProjMatrix(KtMatrix4<float_t>::projectOrtho(box.lower(), box.upper()));
}
