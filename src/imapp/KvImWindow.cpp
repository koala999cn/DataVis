#include "KvImWindow.h"
#include "imgui.h"


KvImWindow::KvImWindow(const std::string_view& name)
    : name_(name)
{

}


KvImWindow::~KvImWindow()
{

}


const std::string_view& KvImWindow::name() const 
{ 
    return name_; 
}


bool KvImWindow::visible() const
{
    return visible_;
}


void KvImWindow::setVisible(bool b)
{
    visible_ = b;
}


void KvImWindow::toggleVisibility()
{
    visible_ = !visible_;
}


bool KvImWindow::begin()
{
    onBegin();
    bool keep_visible{ true };
    ImGui::SetNextWindowSizeConstraints(
        ImVec2{ minSize_[0], minSize_[1] },
        ImVec2{ maxSize_[0], maxSize_[1] }
    );
    const bool not_collapsed = ImGui::Begin(
        name().data(),
        &keep_visible,
        flags()
    );

    if (!keep_visible)
        setVisible(false);
    return not_collapsed;
}


void KvImWindow::end()
{
    onEnd();
    ImGui::End();
}


void KvImWindow::onBegin()
{

}


void KvImWindow::onEnd()
{

}


int KvImWindow::flags()
{
    return 0; //ImGuiWindowFlags_NoCollapse;
}


bool KvImWindow::consumesMouseInput() const
{
    return false;
}


bool KvImWindow::hasToolbar() const
{
    return false;
}


void KvImWindow::toolbar()
{

}
