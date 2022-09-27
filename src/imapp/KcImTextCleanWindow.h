#pragma once
#include "KvImModalWindow.h"
#include <vector>


// 加载text数据并根据用户配置进行清洗

class KcImTextCleanWindow : public KvImModalWindow
{
public:
    template<typename T>
    using matrix = std::vector<std::vector<T>>;

    KcImTextCleanWindow(const std::string& source, matrix<std::string>& rawData);

    const char* type() const override { return "ImTextCleanWindow"; }

private:
    void updateImpl_() override;

    // 更新统计数据
    void updateStats_();

    // 是否忽略非法字串
    bool skipIllegal_();

private:
    const std::string& source_;
    matrix<std::string>& rawData_; // 原始数据
    matrix<std::string> cleanData_; // 清洗后的数据

    // 解析text文件时的配置项
    int illegalMode_{ 0 }; // 如何处理非数字字串
    int emptyMode_{ 0 }; // 如何处理空字串
    bool forceAlign_{ true }; // 强制列对齐？(当列数量不一致时，取最大列数为数据列数)

    // text数据的一些统计结果，由updateStats_负责更新
    int rows_;
    int emptyTokens_{ 0 };
    int illegalTokens_{ 0 };
    int minCols_, maxCols_{ 0 };
    bool parseFailed_{ true }; // 解析失败？
};
