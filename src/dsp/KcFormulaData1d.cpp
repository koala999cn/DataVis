#include "KcFormulaData1d.h"
#include <algorithm>
#include "exprtkX/KvExprtk.h"


kReal KcFormulaData1d::step(kIndex axis) const
{
    return axis == 0 ? samp_.dx() : k_nonuniform_step;
}


void KcFormulaData1d::clear()
{
    samp_.reset(samp_.xmin(), samp_.xmin(), samp_.dx(), samp_.x0());
}


bool KcFormulaData1d::empty() const
{
    return samp_.empty();
}


kIndex KcFormulaData1d::count() const
{
    return samp_.count();
}


kIndex KcFormulaData1d::length(kIndex axis) const
{
    assert(axis == 0);
    return samp_.count();
}


kRange KcFormulaData1d::range(kIndex axis) const
{
    return axis == 0 ? kRange{ samp_.xmin(), samp_.xmax() } : valueRange();
}


KcFormulaData1d::kPoint2d KcFormulaData1d::value(kIndex idx, kIndex) const
{
    auto x = samp_.indexToX(idx);
    return kPoint2d{ x, expr_->value(x) };
}

