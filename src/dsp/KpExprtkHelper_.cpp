#pragma once
#include "KtContinuedExpr.h"
#include "../3rdparty/exprtkX/KvExprtk.h"


namespace kPrivate
{
    kReal KpExprtkHelper_::operator()(kReal x) const {
        return expr_->value(x);
    }

    kReal KpExprtkHelper_::operator()(kReal x, kReal y) const {
        return expr_->value(x, y);
    }
 
    kReal KpExprtkHelper_::operator()(kReal x, kReal y, kReal z) const {
        return expr_->value(x, y, z);
    }
};
