#pragma once
#include "KvImModalWindow.h"
#include <vector>


// ����text���ݲ������û����ý�����ϴ

class KcImTextCleanWindow : public KvImModalWindow
{
public:
    template<typename T>
    using matrix = std::vector<std::vector<T>>;

    KcImTextCleanWindow(const std::string& source, matrix<std::string>& rawData);

    const char* type() const override { return "ImTextCleanWindow"; }

private:
    void updateImpl_() override;

    // ����ͳ������
    void updateStats_();

    // �Ƿ���ԷǷ��ִ�
    bool skipIllegal_();

private:
    const std::string& source_;
    matrix<std::string>& rawData_; // ԭʼ����
    matrix<std::string> cleanData_; // ��ϴ�������

    // ����text�ļ�ʱ��������
    int illegalMode_{ 0 }; // ��δ���������ִ�
    int emptyMode_{ 0 }; // ��δ�����ִ�
    bool forceAlign_{ true }; // ǿ���ж��룿(����������һ��ʱ��ȡ�������Ϊ��������)

    // text���ݵ�һЩͳ�ƽ������updateStats_�������
    int rows_;
    int emptyTokens_{ 0 };
    int illegalTokens_{ 0 };
    int minCols_, maxCols_{ 0 };
    bool parseFailed_{ true }; // ����ʧ�ܣ�
};
