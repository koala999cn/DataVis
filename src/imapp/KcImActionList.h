#pragma once
#include "KvImWindow.h"
#include <memory>
#include <vector>

class KvAction;

// ������ʾprovider, operator, render���ఴť�Ĺ�����
// �������KcImNodeEditor������Ӧ�½ڵ�

class KcImActionList : public KvImWindow
{
public:

    KcImActionList(const std::string_view& name);

    const char* id() const override { return "ActionList"; }

    void draw() override;


private:
    using action_ptr = std::shared_ptr<KvAction>;

    void drawActionButtons_(std::vector<action_ptr>& actions);

    void trigger_(action_ptr act);

private:
    std::vector<action_ptr> providers_;

    std::vector<action_ptr> triggered_;
};
