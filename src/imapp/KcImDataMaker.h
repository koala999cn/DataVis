#pragma once
#include "KvImModalWindow.h"
#include <vector>
#include <memory>

class KvData;

class KcImDataMaker : public KvImModalWindow
{
public:
    using matrixd = std::vector<std::vector<double>>;

    // @source: 数据来源，用来获取title
    // @idata: 输入的matrix数据
    // @odata: 根据用户配置生成的数据
    KcImDataMaker(const std::string& source, const matrixd& idata, std::shared_ptr<KvData>& odata);


private:
    void updateImpl_() override;

private:
    const matrixd& idata_;
    std::shared_ptr<KvData>& odata_;
    std::vector<char> vis_; // 各列数据的可见性

    bool transpose_{ false }; // 若true，对数据进行转置

    std::vector<int> typeList_; // 可用的数据类型列表，用来生成combo
    int dataType_{ 0 }; // 期望生成的数据类型
};
