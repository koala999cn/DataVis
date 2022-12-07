#include "KvPlot2d.h"
#include "KvCoord.h"


void KvPlot2d::autoProject_()
{
    auto box = coord().boundingBox();
    setProjMatrix(KtMatrix4<float_t>::projectOrtho(box.lower(), box.upper()));
}