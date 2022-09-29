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


int KvImWindow::flags() const
{
    return 0; //ImGuiWindowFlags_NoCollapse;
}


void KvImWindow::update()
{
    ImGui::SetNextWindowSizeConstraints(
        ImVec2{ minSize_[0], minSize_[1] },
        ImVec2{ maxSize_[0], maxSize_[1] }
    );

    if (ImGui::Begin(name().c_str(), &visible_, flags()))
        updateImpl_();

    ImGui::End();
}
