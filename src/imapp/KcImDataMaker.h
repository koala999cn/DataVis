#pragma once
#include "KvImModalWindow.h"
#include <vector>
#include <memory>

class KvData;

class KcImDataMaker : public KvImModalWindow
{
public:
    using matrixd = std::vector<std::vector<double>>;

    // @source: ������Դ��������ȡtitle
    // @idata: �����matrix����
    // @odata: �����û��������ɵ�����
    KcImDataMaker(const std::string& source, const matrixd& idata, std::shared_ptr<KvData>& odata);


private:
    void updateImpl_() override;

private:
    const matrixd& idata_;
    std::shared_ptr<KvData>& odata_;

    bool rowMajor_{ false }; // ��true��ÿ�д���һ���������У�ȱʡfalse��ÿ�д���һ����������

    std::vector<int> typeList_; // ���õ����������б�����������combo
    int dataType_{ 0 }; // �������ɵ���������
};