#pragma once
#include <memory>
#include <stdarg.h>
#include "KtContinued.h"


// 基于exprtk的连续数据实现

class KvExprtk;

namespace kPrivate
{
    struct KpExprtkHelper_ {
        KpExprtkHelper_(std::shared_ptr<KvExprtk> expr)
            : expr_(expr) {}

        kReal operator()(kReal x, ...) const {
            va_list args;
            va_start(args, x);
            kReal r = expr_->value(x, args);
            va_end(args);
            return r;
        }

    private:
        std::shared_ptr<KvExprtk> expr_;
    };
};

template<int DIM>
class KtContinuedExpr : public KtContinued<kPrivate::KpExprtkHelper_, DIM>
{
public:
    using super_ = KtContinued<kPrivate::KpExprtkHelper_, DIM>;

    KtContinuedExpr(std::shared_ptr<KvExprtk> expr)
        : super_(expr) {}
};

