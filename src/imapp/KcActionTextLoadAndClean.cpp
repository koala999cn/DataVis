#include "KcActionTextLoadAndClean.h"
#include "KcImTextCleaner.h"
#include <regex>
#include <fstream>
#include <assert.h>
#include "KuStrUtil.h"
#include "KuFileUtil.h"


KcActionTextLoadAndClean::KcActionTextLoadAndClean(const std::string& filepath)
    : KvAction("DataClean")
    , filepath_(filepath)
{

}


bool KcActionTextLoadAndClean::trigger()
{
    // 加载数据文件
    if (!loadData_()) {
        state_ = KeState::k_failed;
        return false;
    }

    if (rawData_.empty()) { // 空文件
        return false;
    }
    
    // 创建数据窗口
    cleanData_.clear();
    cleanWindow_ = std::make_unique<KcImTextCleaner>(filepath_, rawData_, cleanData_);
    if (cleanWindow_ == nullptr) {
        state_ = KeState::k_failed;
        return false;
    }

    state_ = KeState::k_triggered;
    return true;
}


void KcActionTextLoadAndClean::update()
{
    assert(cleanWindow_ != nullptr);

    if (cleanWindow_->visible())
        cleanWindow_->update();
    else {
        state_ = cleanData_.empty() ? KeState::k_cancelled : KeState::k_done;  
        cleanWindow_ = nullptr;
    }
}


namespace kPrivate
{
    // 返回0表示无引号，否则返回"或者'
    int testQuote(const std::vector<std::string_view>& lines)
    {
        int lineTested(0); // 已检测的有效行数，只检测前2行（不含注释）
        int quote(0);
        for (auto& line : lines) {

            auto tl = KuStrUtil::trim(line);
            if (tl.empty() || tl[0] == '#' || tl[0] == '!')
                continue; // 跳过空行与注释

            auto pos = tl.find_first_of("'\"");
            if (pos != std::string_view::npos) {
                quote = tl[pos];
                break;
            }

            if (++lineTested == 2)
                break;
        }

        return quote;
    }

    template<bool PARSE_QUOTE>
    std::vector<std::vector<std::string_view>> parseLines(const std::vector<std::string_view>& lines)
    {
        std::vector<std::vector<std::string_view>> mat;
        mat.reserve(lines.size());

        for (auto& line : lines) {

            auto tl = KuStrUtil::trim(line);
            if (tl.empty()) {
                mat.push_back({}); // 插入空行
            }
            else if (tl[0] != '#' && tl[0] != '!') {
                assert(tl[0] != '\0');
                std::vector<std::string_view> toks;

                if constexpr (PARSE_QUOTE) 
                    toks = KuStrUtil::splitWithQuote(tl, ", \t", true);
                else 
                    toks = KuStrUtil::split(tl, ", \t", true);

                mat.emplace_back(std::move(toks));
            }
            else {
                ; // 跳过注释行
            }
        }

        return mat;
    }
}


bool KcActionTextLoadAndClean::loadData_()
{
    text_ = KuFileUtil::readAsString(filepath_); // 暂存所有文本，以便后面使用string_view类型
    auto lines = KuStrUtil::split(text_, "\n", false); // 空行有特殊语义，保留

    int quote = kPrivate::testQuote(lines); // 是否解析引号

    if (quote)
        rawData_ = kPrivate::parseLines<true>(lines);
    else 
        rawData_ = kPrivate::parseLines<false>(lines);

    // 删除开头和最后的空行
    while (!rawData_.empty() && rawData_.back().empty())
        rawData_.pop_back();

    while (!rawData_.empty() && rawData_.front().empty())
        rawData_.erase(rawData_.cbegin());

    return true;
}