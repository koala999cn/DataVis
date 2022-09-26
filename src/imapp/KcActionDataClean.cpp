#include "KcActionDataClean.h"
#include "KcImDataView.h"
#include <regex>
#include <fstream>
#include "KuStrUtil.h"


KcActionDataClean::KcActionDataClean(const std::string& filepath)
    : KvAction("DataClean")
    , filepath_(filepath)
{

}


bool KcActionDataClean::trigger()
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
    dataView_ = std::make_unique<KcImDataView>(filepath_, rawData_);
    if (dataView_ == nullptr) {
        state_ = KeState::k_failed;
        return false;
    }

    state_ = KeState::k_triggered;
    return true;
}


void KcActionDataClean::update()
{
    if (dataView_ == nullptr)
        return;

    if (dataView_->opened())
        dataView_->update();
    else 
        state_ = rawData_.empty() ? KeState::k_cancelled : KeState::k_done;
}


bool KcActionDataClean::loadData_()
{
    const std::string rexpNA = "na|nan|n/a|-";
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