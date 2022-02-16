#pragma once
#include "KvExprtk.h"


class EXPRTKX_EXPORT KcExprtk1d : public KvExprtk
{
public:
    KcExprtk1d();

    REAL value(REAL x, ...) const override;

protected:
    mutable REAL x_;
};
