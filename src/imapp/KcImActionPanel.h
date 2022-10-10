#pragma once
#include "KvImWindow.h"
#include <memory>
#include <unordered_map>

class KvAction;

// 一个分组显示action触发按钮列表的Im窗口实现

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
    using action_group_map = std::unordered_map<std::string, action_list>; // 分组名映射到action列表
    
    action_group_map groupMaps_; // 按分组组织的action列表
    action_list triggered_; // 处于触发状态的actions
};
