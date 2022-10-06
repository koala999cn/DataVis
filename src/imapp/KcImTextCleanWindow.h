#pragma once
#include "KvImModalWindow.h"
#include <vector>
#include <set>


// ����text���ݲ������û����ý�����ϴ���������ݶ���

class KcImTextCleanWindow : public KvImModalWindow
{
public:
    template<typename T>
    using matrix = std::vector<std::vector<T>>;

    // @source: ������Դ��������ȡtitle
    // @rawData: ����ϴ��ԭʼ����
    // @cleanData: ����������ϴ���
    KcImTextCleanWindow(const std::string& source, const matrix<std::string>& rawData, matrix<double>& cleanData);

    //const char* type() const override { return "ImTextCleanWindow"; }

private:
    void updateImpl_() override;

    // ����ͳ������
    void updateStats_();

    // �Ƿ���ԷǷ��ִ�
    bool skipIllegal_() const;

    // �����û����ã�ִ��������ϴ����
    void clean_();

    // ����empty�ִ���ȡֵ
    double emptyValue_(const std::string& tok) const;

    // ���طǷ��ִ���ȡֵ
    double illegalValue_(const std::string& tok) const;

    void showTable_() const;

private:
    const std::string& source_;
    const matrix<std::string>& rawData_; // ԭʼ����
    matrix<double>& cleanData_; // ��ϴ�������

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
    std::set<int> emptyLines_; // ���п����к�
};
