#include "KcImNodeEditor.h"
#include "imnodes/imnodes.h"
#include <assert.h>


KcImNodeEditor::KcImNodeEditor(const std::string_view& name)
    : KvImWindow(name)
{
    // test graph
    graph_.addVertex(std::make_shared<KvNode>("test node1", 2, 3));
    graph_.addVertex(std::make_shared<KvNode>("test node2", 0, 1));
    graph_.addVertex(std::make_shared<KvNode>("test node3", 1, 0));
}


void KcImNodeEditor::update()
{
    ImNodes::BeginNodeEditor();

    for (unsigned v = 0; v < graph_.order(); v++) {
     
        auto node = graph_.vertexAt(v)->as<KvNode*>();
        if (!node)
            continue; // 只轮询主节点，端口会在主节点绘制时考虑

        ImNodes::BeginNode(node->id());

        ImNodes::BeginNodeTitleBar();
        const char* text = node->name().data();
        ImGui::TextUnformatted(text, text + node->name().size());
        ImNodes::EndNodeTitleBar();

        for (unsigned i = 0; i < node->ins(); i++) {
            auto& port = node->inPort(i);
            ImNodes::BeginInputAttribute(port.id());
            ImGui::Text(port.name().c_str());
            ImNodes::EndInputAttribute();
        }

        ImGui::Spacing();

        for (unsigned i = 0; i < node->outs(); i++) {
            auto& port = node->outPort(i);
            ImNodes::BeginOutputAttribute(port.id());
            //ImGui::Indent(40); // TODO
            ImGui::Text(port.name().c_str());
            ImNodes::EndOutputAttribute();
        }

        ImNodes::EndNode();
    }


    ImNodes::EndNodeEditor();
}


unsigned KcImNodeEditor::vertex2Index_(const vertex_type& v) const
{
    assert(false);
    return 0;
}