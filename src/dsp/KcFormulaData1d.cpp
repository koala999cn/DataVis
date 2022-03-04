#include "KcFormulaData1d.h"
#include <algorithm>
#include "exprtkX/KvExprtk.h"


kReal KcFormulaData1d::step(int axis) const
{
    return axis == 0 ? samp_.dx() : k_nonuniform_step;
}


void KcFormulaData1d::clear()
{
    samp_.reset(samp_.xmin(), samp_.xmin(), samp_.dx(), samp_.x0());
}


kIndex KcFormulaData1d::count() const
{
    return samp_.nx();
}


kRange KcFormulaData1d::xrange() const
{
    return kRange{ samp_.xmin(), samp_.xmax() };
}


kPoint2d KcFormulaData1d::value(kIndex idx, kIndex) const
{
    auto x = samp_.indexToX(idx);
    return kPoint2d{ x, expr_->value(x) };
}
