#pragma once
#include "KtUniObject.h"

// imgui窗口的封装类

class KvImWindow : public KtUniObject<KvImWindow>
{
public:
    using super_ = KtUniObject<KvImWindow>;

    using super_::super_;

    virtual ~KvImWindow() noexcept;

    // 可见性
    bool visible() const;
    void setVisible(bool b);
    void toggleVisibility();

    virtual const char* type() const = 0; // 窗口类型的标识符，确保每类窗口返回值一致
    virtual void update(); // 绘制窗口部件
    virtual void onBegin() {}
    virtual void onEnd() {}
    virtual int flags(); // ImGuiWindowFlags
    virtual bool consumesMouseInput() const { return false; }
    virtual bool hasToolbar() const { return false; }
    virtual void toolbar() {} // 绘制工具栏

private:
    virtual bool begin_();
    virtual void end_();
    virtual void updateImpl_() = 0;

private:
    bool visible_{ true };

    const std::string name_;

    float minSize_[2]{ 120.0f, 120.0f };
    float maxSize_[2]{ 99999.0f, 99999.0f };
};

