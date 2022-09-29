#pragma once
#include "KvImModalWindow.h"
#include <vector>
#include <memory>

class KvData;

class KcImDataView : public KvImModalWindow
{
public:
    using matrixd = std::vector<std::vector<double>>;

    // @source: ������Դ��������ȡtitle
    // @idata: �����matrix����
    // @odata: �����û��������ɵ�����
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

	// ��matת��Ϊtype��������
	// ����row-major��ȡmat����
	static std::shared_ptr<KvData> makeData_(const matrixd& mat, int type);

private:
    const matrixd& idata_;
    std::shared_ptr<KvData>& odata_;

    bool rowMajor_{ false }; // ��true��ÿ�д���һ���������У�ȱʡfalse��ÿ�д���һ����������

    std::vector<int> typeList_; // ���õ����������б���������combo
    int dataType_{ 0 }; // �������ɵ���������
};
