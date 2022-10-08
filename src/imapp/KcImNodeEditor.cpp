#include "KcImNodeEditor.h"
#include "imnodes/imnodes.h"
#include <assert.h>
#include "kgl/core/KtBfsIter.h"


KcImNodeEditor::KcImNodeEditor(const std::string_view& name)
    : KvImWindow(name)
{

}


void KcImNodeEditor::updateImpl_()
{
    testNewLink_();

    ImNodes::BeginNodeEditor();

    drawNodes_();
    drawLinks_();

    ImNodes::MiniMap(0.2, ImNodesMiniMapLocation_BottomRight);

    ImNodes::EndNodeEditor();
}


void KcImNodeEditor::drawNodes_() const
{
    for (unsigned v = 0; v < graph_.order(); v++) {

        auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
        assert(node); // block节点断言

        ImNodes::BeginNode(node->id());

        float titleWidth = ImGui::CalcTextSize(node->name().c_str()).x;

        ImNodes::BeginNodeTitleBar();
        const char* text = node->name().c_str();
        ImGui::TextUnformatted(text, text + node->name().size());
        ImNodes::EndNodeTitleBar();

        float maxInputWidth(0);

        // 端口节点的index必然与bolck节点连续
        for (unsigned i = 0; i < node->inPorts(); i++) {
            auto& port = graph_.vertexAt(++v);

            auto thisWidth = ImGui::CalcTextSize(port->name().c_str()).x;
            if (thisWidth > maxInputWidth)
                maxInputWidth = thisWidth;

            ImNodes::BeginInputAttribute(port->id());
            ImGui::Text(port->name().c_str());
            ImNodes::EndInputAttribute();
        }

        if (node->inPorts() > 0)
            ImGui::Spacing();


        for (unsigned i = 0; i < node->outPorts(); i++) {
            auto& port = graph_.vertexAt(++v);
            ImNodes::BeginOutputAttribute(port->id());

            auto thisWidth = ImGui::CalcTextSize(port->name().c_str()).x;

            if (thisWidth < titleWidth)
                ImGui::Indent(titleWidth - thisWidth);
            else
                ImGui::Indent(maxInputWidth + 2);

            ImGui::Text(port->name().c_str());
            ImNodes::EndOutputAttribute();
        }

        ImNodes::EndNode();
    }
}


void KcImNodeEditor::drawLinks_() const
{
    if (graph_.size() > 0) {
        KtBfsIter<const node_graph, true, true> iter(graph_, 0);
        for (; !iter.isEnd(); ++iter) {
            auto& from = graph_.vertexAt(iter.from());
            auto& to = graph_.vertexAt(*iter);
            ImNodes::Link(linkId_(from->id(), to->id()), from->id(), to->id());
        }
    }
}


void KcImNodeEditor::insertNode(const std::shared_ptr<KvBlockNode>& node)
{
    graph_.addVertex(node);

    // 构造输入端口节点
    for (unsigned i = 0; i < node->inPorts(); i++) 
        graph_.addVertex(std::make_shared<KcPortNode>(KcPortNode::k_in, node, i));


    // 构造输出端口节点
    for (unsigned i = 0; i < node->outPorts(); i++) 
        graph_.addVertex(std::make_shared<KcPortNode>(KcPortNode::k_out, node, i));
}


unsigned KcImNodeEditor::nodeIndex_(const node_ptr& node) const
{
    for (unsigned v = 0; v < graph_.order(); v++)
        if (node == graph_.vertexAt(v))
            return v;

    return -1;
}


unsigned KcImNodeEditor::nodeId2Index_(int id) const
{
    for (unsigned v = 0; v < graph_.order(); v++)
        if (id == graph_.vertexAt(v)->id())
            return v;

    return -1;
}


int KcImNodeEditor::linkId_(int fromId, int toId)
{
    assert(fromId < std::numeric_limits<short>::max() && toId < std::numeric_limits<short>::max());
    return fromId << 16 | toId;
}


void KcImNodeEditor::testNewLink_()
{
    int fromId, toId;
    if (ImNodes::IsLinkCreated(&fromId, &toId)) {
        auto fromIdx = nodeId2Index_(fromId);
        auto toIdx = nodeId2Index_(toId);
        graph_.addEdge(fromIdx, toIdx);
    }
}