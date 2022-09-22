#include "KvImWidget.h"
#include "imgui.h"


KvImWidget::KvImWidget(const std::string_view title)
    : title_(title), label_(title)
{

}


KvImWidget::KvImWidget(const std::string_view title, const std::string_view label)
    : title_(title), label_(label)
{

}


KvImWidget::~KvImWidget()
{

}


const std::string_view KvImWidget::title() const 
{ 
    return title_; 
}


const char* KvImWidget::label() const 
{ 
    return label_.c_str(); 
}


bool KvImWidget::visible() const
{
    return visible_;
}


void KvImWidget::setVisible(bool b)
{
    visible_ = b;
}


void KvImWidget::toggleVisibility()
{
    visible_ = !visible_;
}


bool KvImWidget::begin()
{
    onBegin();
    bool keep_visible{ true };
    ImGui::SetNextWindowSizeConstraints(
        ImVec2{ minSize_[0], minSize_[1] },
        ImVec2{ maxSize_[0], maxSize_[1] }
    );
    const bool not_collapsed = ImGui::Begin(
        label(),
        &keep_visible,
        flags()
    );

    if (!keep_visible)
        setVisible(false);
    return not_collapsed;
}


void KvImWidget::end()
{
    onEnd();
    ImGui::End();
}


void KvImWidget::onBegin()
{

}


void KvImWidget::onEnd()
{

}


int KvImWidget::flags()
{
    return 0; //ImGuiWindowFlags_NoCollapse;
}


bool KvImWidget::consumesMouseInput() const
{
    return false;
}


bool KvImWidget::hasToolbar() const
{
    return false;
}


void KvImWidget::toolbar()
{

}
