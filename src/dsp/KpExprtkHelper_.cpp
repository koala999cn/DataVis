#pragma once
#include "KtContinuedExpr.h"
#include "../3rdparty/exprtkX/KvExprtk.h"


namespace kPrivate
{
    kReal KpExprtkHelper_::operator()(kReal x, ...) const {
        va_list args;
        va_start(args, x);
        kReal r = expr_->value(x, args);
        va_end(args);
        return r;
    }
};
