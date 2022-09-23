#include "KcImNodeEditor.h"
#include "imnodes/imnodes.h"


KcImNodeEditor::KcImNodeEditor(const std::string_view& name)
    : KvImWindow(name)
{

}


void KcImNodeEditor::draw()
{
    ImNodes::BeginNodeEditor();

    ImNodes::BeginNode(1);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("simple node :)");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(2);
    ImGui::Text("input");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(3);
    ImGui::Indent(40);
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();

    ImNodes::EndNodeEditor();
}
