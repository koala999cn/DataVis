#pragma once
#include "KvImModalWindow.h"
#include <vector>
#include <memory>

class KvData;

class KcImDataView : public KvImModalWindow
{
public:
    using matrixd = std::vector<std::vector<double>>;

    // @source: 数据来源，用来获取title
    // @idata: 输入的matrix数据
    // @odata: 根据用户配置生成的数据
    KcImDataView(const std::string& source, const matrixd& idata, std::shared_ptr<KvData>& odata);


private:
    void updateImpl_() override;


private:
    const matrixd& idata_;
    std::shared_ptr<KvData>& odata_;

    bool rowMajor_{ false }; // 若true，每行代表一个数据序列，缺省false，每列代表一个数据序列
    int dataType_{ 0 }; // 需要生成的数据类型
};
