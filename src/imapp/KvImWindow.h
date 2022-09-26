#pragma once
#include "KtUniObject.h"

// imgui���ڵķ�װ��

class KvImWindow : public KtUniObject<KvImWindow>
{
public:
    using super_ = KtUniObject<KvImWindow>;

    using super_::super_;

    virtual ~KvImWindow() noexcept;

    // �ɼ���
    bool visible() const;
    void setVisible(bool b);
    void toggleVisibility();

    virtual const char* type() const { return "ImWindow"; } // �������͵ı�ʶ����ȷ��ÿ�ര�ڷ���ֵһ��  
    virtual int flags() const; // ImGuiWindowFlags
    virtual void update(); // ���ƴ��ڲ���
    virtual bool consumesMouseInput() const { return false; }
    virtual bool hasToolbar() const { return false; }
    virtual void toolbar() {} // ���ƹ�����

protected:
    virtual void updateImpl_() = 0;

private:
    bool visible_{ true };

    float minSize_[2]{ 120.0f, 120.0f };
    float maxSize_[2]{ 99999.0f, 99999.0f };
};

