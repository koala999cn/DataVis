#include "KcActionTextLoadAndClean.h"
#include "KcImTextCleanWindow.h"
#include <regex>
#include <fstream>
#include "KuStrUtil.h"


KcActionTextLoadAndClean::KcActionTextLoadAndClean(const std::string& filepath)
    : KvAction("DataClean")
    , filepath_(filepath)
{

}


bool KcActionTextLoadAndClean::trigger()
{
    // ���������ļ�
    if (!loadData_()) {
        state_ = KeState::k_failed;
        return false;
    }

    if (rawData_.empty()) { // ���ļ�
        return false;
    }
    
    // �������ݴ���
    dataView_ = std::make_unique<KcImTextCleanWindow>(filepath_, rawData_);
    if (dataView_ == nullptr) {
        state_ = KeState::k_failed;
        return false;
    }

    state_ = KeState::k_triggered;
    return true;
}


void KcActionTextLoadAndClean::update()
{
    if (dataView_ == nullptr)
        return;

    if (dataView_->opened())
        dataView_->update();
    else 
        state_ = rawData_.empty() ? KeState::k_cancelled : KeState::k_done;
}


bool KcActionTextLoadAndClean::loadData_()
{
    const std::string rexpNA = "na|nan|n/a";
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