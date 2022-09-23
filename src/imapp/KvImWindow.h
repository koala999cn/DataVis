#pragma once
#include <string_view>

// imgui窗口的封装类

class KvImWindow
{
public:
    explicit KvImWindow(const std::string_view& name);
    virtual ~KvImWindow() noexcept;

    // 可见性
    bool visible() const;
    void setVisible(bool b);
    void toggleVisibility();

    const std::string_view& name() const;

    bool begin();
    void end();

    virtual const char* id() const = 0; // 窗口类型的标识符，确保每类窗口返回值一致
    virtual void draw() = 0; // 绘制窗口部件
    virtual void onBegin();
    virtual void onEnd();
    virtual int flags(); // ImGuiWindowFlags
    virtual bool consumesMouseInput() const;
    virtual bool hasToolbar() const;
    virtual void toolbar(); // 绘制工具栏

protected:
    bool visible_{ true };

    const std::string name_;

    float minSize_[2]{ 120.0f, 120.0f };
    float maxSize_[2]{ 99999.0f, 99999.0f };
};

