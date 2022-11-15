#include "KcExprtk1d.h"
#include "exprtk/exprtk.hpp"

#define exprtk_obj ((exprtk::expression<REAL>*)exprtk_)


KcExprtk1d::KcExprtk1d() : KvExprtk()
{  
    exprtk_obj->get_symbol_table().add_variable("x", x_);
}


REAL KcExprtk1d::value(REAL x[]) const
{
    x_ = x[0];
    return exprtk_obj->value();
}
