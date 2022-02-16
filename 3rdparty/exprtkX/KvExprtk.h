#pragma once
#include "exprtkX.h"
#include <string>


class EXPRTKX_EXPORT KvExprtk
{
public:
    KvExprtk();
    virtual ~KvExprtk();

    virtual REAL value(REAL x, ...) const = 0;

    virtual bool compile(const std::string& expr);

    virtual bool ok() const; // ready to call value(x, ...)

    unsigned vars() const;

    std::string error() const { return error_; }


protected:
    void* exprtk_;
    std::string error_;
};

