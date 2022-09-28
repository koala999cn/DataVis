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


private:
    const matrixd& idata_;
    std::shared_ptr<KvData>& odata_;

    bool rowMajor_{ false }; // ��true��ÿ�д���һ���������У�ȱʡfalse��ÿ�д���һ����������
    int dataType_{ 0 }; // ��Ҫ���ɵ���������
};
