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

    void close() {
        setVisible(false);
        onClose(false);
    }

    // @clicked: 若为true，则表示该事件由用户点击关闭按钮触发
    virtual void onClose(bool clicked) {}

    virtual int flags() const; // ImGuiWindowFlags

    virtual std::string label() const;

    virtual void update(); // 绘制窗口部件

    bool deleteOnClose() const { return deleteOnClose_; }

    bool dynamic() const { return dynamic_; }

protected:
    virtual void updateImpl_() = 0;

protected:
    bool visible_{ true };

    float minSize_[2]{ 120.0f, 120.0f };
    float maxSize_[2]{ 99999.0f, 99999.0f };

    // 几个flag
    bool deleteOnClose_{ false };
    bool dynamic_{ false };
};

