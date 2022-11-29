#include "KvPlot2d.h"
#include "KtAABB.h"


KvPlot2d::KvPlot2d(std::shared_ptr<KvPaint> paint, std::shared_ptr<KvCoord> coord)
	: KvPlot(paint, coord)
{
	coord_ = coord;
}


void KvPlot2d::autoProject_()
{

}