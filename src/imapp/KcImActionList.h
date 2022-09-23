#pragma once
#include "KvImWindow.h"


// ������ʾprovider, operator, render���ఴť�Ĺ�����
// �������KcImNodeEditor������Ӧ�½ڵ�

class KcImActionList : public KvImWindow
{
public:

    KcImActionList(const std::string_view& name);

    const char* id() const override { return "ActionList"; }

    void draw() override;


private:
    void drawProvider_();
    void drawOperator_();
    void drawRenderer_();
};