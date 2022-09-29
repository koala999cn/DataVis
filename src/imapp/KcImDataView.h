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

    void showTable_() const;

    void setupTableHeader_(int cols) const;

    static std::string seriesName_(int idx);

	static std::shared_ptr<KvData> makeSeries_(const matrixd& mat);

	static std::shared_ptr<KvData> makeMatrix_(const matrixd& mat);

	static std::shared_ptr<KvData> makeSampled1d_(const matrixd& mat);

	static std::shared_ptr<KvData> makeSampled2d_(const matrixd& mat);

	static std::shared_ptr<KvData> makeScattered_(const matrixd& mat, unsigned dim);

	// 把mat转换为type类型数据
	// 按照row-major读取mat数据
	static std::shared_ptr<KvData> makeData_(const matrixd& mat, int type);

private:
    const matrixd& idata_;
    std::shared_ptr<KvData>& odata_;

    bool rowMajor_{ false }; // 若true，每行代表一个数据序列，缺省false，每列代表一个数据序列

    std::vector<int> typeList_; // 可用的数据类型列表，用来生成combo
    int dataType_{ 0 }; // 期望生成的数据类型
};
