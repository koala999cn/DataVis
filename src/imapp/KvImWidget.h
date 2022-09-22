#pragma once
#include <string_view>

// imgui窗口的封装类

class KvImWidget
{
public:
    explicit KvImWidget(const std::string_view title);
    KvImWidget(const std::string_view title, const std::string_view label);
    virtual ~KvImWidget() noexcept;

    // 可见性
    bool visible() const;
    void setVisible(bool b);
    void toggleVisibility();

    const std::string_view title() const;
    const char* label() const; // 为适应ImGui::Begin，返回const char*

    bool begin();
    void end();

    virtual void imgui() = 0; // 绘制窗口部件
    virtual void onBegin();
    virtual void onEnd();
    virtual int flags(); // ImGuiWindowFlags
    virtual bool consumesMouseInput() const;
    virtual bool hasToolbar() const;
    virtual void toolbar(); // 绘制工具栏

protected:
    bool visible_{ true };

    const std::string title_;
    const std::string label_;

    float minSize_[2]{ 120.0f, 120.0f };
    float maxSize_[2]{ 99999.0f, 99999.0f };
};

