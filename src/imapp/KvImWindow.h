#pragma once
#include <string_view>

// imgui���ڵķ�װ��

class KvImWindow
{
public:
    explicit KvImWindow(const std::string_view& name);
    virtual ~KvImWindow() noexcept;

    // �ɼ���
    bool visible() const;
    void setVisible(bool b);
    void toggleVisibility();

    const std::string_view& name() const;

    bool begin();
    void end();

    virtual const char* id() const = 0; // �������͵ı�ʶ����ȷ��ÿ�ര�ڷ���ֵһ��
    virtual void draw() = 0; // ���ƴ��ڲ���
    virtual void onBegin();
    virtual void onEnd();
    virtual int flags(); // ImGuiWindowFlags
    virtual bool consumesMouseInput() const;
    virtual bool hasToolbar() const;
    virtual void toolbar(); // ���ƹ�����

protected:
    bool visible_{ true };

    const std::string name_;

    float minSize_[2]{ 120.0f, 120.0f };
    float maxSize_[2]{ 99999.0f, 99999.0f };
};

