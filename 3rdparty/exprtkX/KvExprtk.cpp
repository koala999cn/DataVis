#include "KvExprtk.h"
#include "exprtk/exprtk.hpp"


#define exprtk_obj ((exprtk::expression<REAL>*)exprtk_)


KvExprtk::KvExprtk()
{
    exprtk_ = new exprtk::expression<REAL>;
    exprtk::symbol_table<REAL> symbol_table;
    symbol_table.add_constants();
    exprtk_obj->register_symbol_table(symbol_table);
}


KvExprtk::~KvExprtk()
{
    delete exprtk_obj;
}


bool KvExprtk::compile(const std::string& expr)
{
    exprtk::parser<REAL> parser;
    error_.clear();
    if(!parser.compile(expr.c_str(), *exprtk_obj)) {
        error_ = parser.error();
        return false;
    }

    return true;
}


bool KvExprtk::ok() const
{
    return error_.empty();
}


unsigned KvExprtk::vars() const
{
    return static_cast<unsigned>(exprtk_obj->get_symbol_table().variable_count());
}

