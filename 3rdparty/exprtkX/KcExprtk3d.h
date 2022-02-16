#pragma once
#include "KcExprtk2d.h"


class KcExprtk3d : public KcExprtk2d
{
public:
    KcExprtk3d();

    REAL value(REAL x, ...) const override;

protected:
    mutable REAL z_;
};


