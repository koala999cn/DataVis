#pragma once
#include "KcExprtk1d.h"


class EXPRTKX_EXPORT KcExprtk2d : public KcExprtk1d
{
public:
    KcExprtk2d();

    REAL value(REAL x[]) const override;

protected:
    mutable REAL y_;
};

