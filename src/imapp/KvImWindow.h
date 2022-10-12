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

    //virtual const char* type() const { return "ImWindow"; } // 窗口类型的标识符，确保每类窗口返回值一致  
    
    virtual int flags() const; // ImGuiWindowFlags

    virtual std::string label() const;

    virtual void update(); // 绘制窗口部件

protected:
    virtual void updateImpl_() = 0;

protected:
    bool visible_{ true };

    float minSize_[2]{ 120.0f, 120.0f };
    float maxSize_[2]{ 99999.0f, 99999.0f };
};

