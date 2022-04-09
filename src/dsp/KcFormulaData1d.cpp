#include "KcFormulaData1d.h"
#include <algorithm>
#include "exprtkX/KvExprtk.h"


kReal KcFormulaData1d::step(kIndex axis) const
{
    return axis == 0 ? samp_.dx() : k_nonuniform_step;
}


void KcFormulaData1d::clear()
{
    samp_.clear();
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


kReal KcFormulaData1d::value(kIndex idx[], kIndex channel) const
{
    auto x = samp_.indexToX(idx[0]);
    return value(&x, channel);
}


std::vector<kReal> KcFormulaData1d::point(kIndex idx[], kIndex channel) const
{
    return { samp_.indexToX(idx[0]), value(idx, channel) };
}


kReal KcFormulaData1d::value(kReal pt[], kIndex channel) const
{
    return expr_->value(pt[0]);
}
