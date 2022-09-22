#pragma once
#include <string_view>

// imgui���ڵķ�װ��

class KvImWidget
{
public:
    explicit KvImWidget(const std::string_view title);
    KvImWidget(const std::string_view title, const std::string_view label);
    virtual ~KvImWidget() noexcept;

    // �ɼ���
    bool visible() const;
    void setVisible(bool b);
    void toggleVisibility();

    const std::string_view title() const;
    const char* label() const; // Ϊ��ӦImGui::Begin������const char*

    bool begin();
    void end();

    virtual void imgui() = 0; // ���ƴ��ڲ���
    virtual void onBegin();
    virtual void onEnd();
    virtual int flags(); // ImGuiWindowFlags
    virtual bool consumesMouseInput() const;
    virtual bool hasToolbar() const;
    virtual void toolbar(); // ���ƹ�����

protected:
    bool visible_{ true };

    const std::string title_;
    const std::string label_;

    float minSize_[2]{ 120.0f, 120.0f };
    float maxSize_[2]{ 99999.0f, 99999.0f };
};

