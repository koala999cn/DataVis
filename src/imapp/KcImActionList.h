#pragma once
#include "KvImWindow.h"


// 用于显示provider, operator, render各类按钮的工具栏
// 点击后将向KcImNodeEditor增加相应新节点

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