#pragma once
#include "KvImWindow.h"
#include <vector>


class KcImDataView : public KvImWindow
{
public:
    template<typename T>
    using matrix = std::vector<std::vector<T>>;

    KcImDataView(const std::string& source, const matrix<std::string>& rawData);

    const char* type() const override { return "DataView"; }

    int flags() override;

private:
    void updateImpl_() override;

private:
    const std::string& source_;
    const matrix<std::string>& rawData_;
};
