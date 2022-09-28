#include "KcActionTextLoadAndClean.h"
#include "KcImTextCleanWindow.h"
#include <regex>
#include <fstream>
#include <assert.h>
#include "KuStrUtil.h"


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
    cleanWindow_ = std::make_unique<KcImTextCleanWindow>(filepath_, rawData_, cleanData_);
    if (cleanWindow_ == nullptr) {
        state_ = KeState::k_failed;
        return false;
    }

    state_ = cleanWindow_->visible() ? KeState::k_triggered : KeState::k_done;
    return true;
}


void KcActionTextLoadAndClean::update()
{
    assert(cleanWindow_ != nullptr);

    if (cleanWindow_->visible())
        cleanWindow_->update();
    else 
        state_ = cleanData_.empty() ? KeState::k_cancelled : KeState::k_done;
}


bool KcActionTextLoadAndClean::loadData_()
{
    const std::string rexpDelim = "\\s+";

    rawData_.clear();

    std::ifstream ifs(filepath_);
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty())
            continue;

        auto tokens = KuStrUtil::splitRegex(line, rexpDelim, false);
        if (tokens.empty())
            continue; // always skip empty line

        rawData_.emplace_back(std::move(tokens));
    }

    return true;
}