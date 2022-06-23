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

        kReal operator()(kReal x) const;
        kReal operator()(kReal x, kReal y) const;
        kReal operator()(kReal x, kReal y, kReal z) const;

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

