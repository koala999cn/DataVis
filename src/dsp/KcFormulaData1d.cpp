#include "KcFormulaData1d.h"
#include <algorithm>
#include "exprtkX/KvExprtk.h"


KcFormulaData1d::KcFormulaData1d(kReal xmin, kReal xmax, kIndex nx, const std::shared_ptr<KvExprtk>& expr)
    : expr_(expr), KtSampled<1>()
{
    KtSampled<1>::reset(0, xmin, (xmax - xmin) / nx, 0.5);
    assert(count() == nx);
}


kReal KcFormulaData1d::value(kIndex idx[], kIndex) const
{
    kReal pt = sampling(0).indexToX(idx[0]);
    return value(&pt, 0);
}


kReal KcFormulaData1d::value(kReal pt[], kIndex channel) const
{
    return expr_->value(pt[0]);
}
