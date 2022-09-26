#include "KvImWindow.h"
#include "imgui.h"


KvImWindow::~KvImWindow()
{

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


bool KvImWindow::begin_()
{
    onBegin();
    bool keep_visible{ true };
    ImGui::SetNextWindowSizeConstraints(
        ImVec2{ minSize_[0], minSize_[1] },
        ImVec2{ maxSize_[0], maxSize_[1] }
    );

    ImGui::PushID(id_.id());
    const bool not_collapsed = ImGui::Begin(
        name().data(),
        &keep_visible,
        flags()
    );

    if (!keep_visible)
        setVisible(false);
    return not_collapsed;
}


void KvImWindow::end_()
{
    onEnd();
    ImGui::End();
    ImGui::PopID();
}


int KvImWindow::flags()
{
    return 0; //ImGuiWindowFlags_NoCollapse;
}


void KvImWindow::update()
{
    if (begin_() && visible())
        updateImpl_();
    end_();
}
