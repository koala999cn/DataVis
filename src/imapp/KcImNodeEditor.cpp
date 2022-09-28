#include "KcImNodeEditor.h"
#include "imnodes/imnodes.h"
#include <assert.h>


KcImNodeEditor::KcImNodeEditor(const std::string_view& name)
    : KvImWindow(name)
{

}


void KcImNodeEditor::updateImpl_()
{
    ImNodes::BeginNodeEditor();

    for (unsigned v = 0; v < graph_.order(); v++) {
     
        auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
        if (!node)
            continue; // ֻ��ѯblock�ڵ㣬�˿ڻ������ڵ����ʱ����

        ImNodes::BeginNode(node->id());

        ImNodes::BeginNodeTitleBar();
        const char* text = node->name().c_str();
        ImGui::TextUnformatted(text, text + node->name().size());
        ImNodes::EndNodeTitleBar();

        auto w = node2Index_(node); // �˿ڽڵ��index��Ȼ��bolck�ڵ�����
        for (unsigned i = 0; i < node->inPorts(); i++) {
            auto& port = graph_.vertexAt(++w);
            ImNodes::BeginInputAttribute(port->id());
            ImGui::Text(port->name().c_str());
            ImNodes::EndInputAttribute();
        }

        ImGui::Spacing();

        for (unsigned i = 0; i < node->outPorts(); i++) {
            auto& port = graph_.vertexAt(++w);
            ImNodes::BeginOutputAttribute(port->id());
            ImGui::Indent(40); // TODO:
            ImGui::Text(port->name().c_str());
            ImNodes::EndOutputAttribute();
        }

        ImNodes::EndNode();
    }


    ImNodes::EndNodeEditor();
}


void KcImNodeEditor::insertNode(const std::shared_ptr<KvBlockNode>& node)
{
    graph_.addVertex(node);

    // ��������˿ڽڵ�
    for (unsigned i = 0; i < node->inPorts(); i++) 
        graph_.addVertex(std::make_shared<KcPortNode>(KcPortNode::k_in, node, i));


    // ��������˿ڽڵ�
    for (unsigned i = 0; i < node->outPorts(); i++) 
        graph_.addVertex(std::make_shared<KcPortNode>(KcPortNode::k_out, node, i));
}


unsigned KcImNodeEditor::node2Index_(const node_ptr& node) const
{
    for (unsigned v = 0; v < graph_.order(); v++)
        if (node == graph_.vertexAt(v))
            return v;

    return -1;
}