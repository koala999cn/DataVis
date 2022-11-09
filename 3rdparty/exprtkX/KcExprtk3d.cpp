#include "KcExprtk3d.h"
#include "exprtk/exprtk.hpp"

#define exprtk_obj ((exprtk::expression<REAL>*)exprtk_)


KcExprtk3d::KcExprtk3d() : KcExprtk2d()
{
    exprtk_obj->get_symbol_table().add_variable("z", z_);
}


REAL KcExprtk3d::value(REAL x[]) const
{
    x_ = x[0];
    y_ = x[1];
    z_ = x[2];

    return exprtk_obj->value();
}
