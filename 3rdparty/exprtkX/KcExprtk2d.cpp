#include "KcExprtk2d.h"
#include "exprtk/exprtk.hpp"

#define exprtk_obj ((exprtk::expression<REAL>*)exprtk_)


KcExprtk2d::KcExprtk2d() : KcExprtk1d()
{
    exprtk_obj->get_symbol_table().add_variable("y", y_);
}


REAL KcExprtk2d::value(REAL x[]) const
{
    x_ = x[0];
    y_ = x[1];

    return exprtk_obj->value();
}
