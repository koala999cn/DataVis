#pragma once
#include "KvImWindow.h"
#include <memory>
#include <unordered_map>

class KvAction;

// һ��������ʾaction������ť�б��Im����ʵ��

class KcImActionPanel : public KvImWindow
{
public:

    using action_ptr = std::shared_ptr<KvAction>;

    KcImActionPanel(const std::string_view& name);

    void addAction(const std::string_view& groupName, action_ptr act);

private:
    void updateImpl_() override;

private:
    
    using action_list = std::vector<action_ptr>;
    using action_group_map = std::unordered_map<std::string, action_list>; // ������ӳ�䵽action�б�
    
    action_group_map groupMaps_; // ��������֯��action�б�
    action_list triggered_; // ���ڴ���״̬��actions
};
