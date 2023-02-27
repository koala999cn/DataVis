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
    std::vector<char> vis_; // �������ݵĿɼ���

    bool transpose_{ false }; // ��true�������ݽ���ת��

    std::vector<int> typeList_; // ���õ����������б���������combo
    int dataType_{ 0 }; // �������ɵ���������
};
