#pragma once
#include "KvImModalWindow.h"
#include <vector>


class KcImDataView : public KvImModalWindow
{
public:
    template<typename T>
    using matrix = std::vector<std::vector<T>>;

    KcImDataView(const std::string& source, matrix<std::string>& rawData);

    const char* type() const override { return "DataView"; }

private:
    void updateImpl_() override;

private:
    const std::string& source_;
    matrix<std::string>& rawData_;
};
