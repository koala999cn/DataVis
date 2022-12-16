#include "KvPlot2d.h"
#include "KvCoord.h"


KvPlot2d::KvPlot2d(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord)
    : KvPlot(paint, coord)
{
    setMargins(15, 15, 15, 15);
}


void KvPlot2d::autoProject_()
{
    auto box = coord().boundingBox();
    //assert(box.width() != 0 && box.height() != 0);
    if (box.depth() == 0)
        box.setExtent(2, 1); // ��ֹz��߶�Ϊ0�����򹹽�͸�Ӿ�����nanֵ
    setProjMatrix(KtMatrix4<float_t>::projectOrtho(box.lower(), box.upper()));
}
