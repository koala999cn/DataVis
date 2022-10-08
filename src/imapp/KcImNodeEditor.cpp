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
    // set the titlebar color for all nodes
    ImNodesStyle& style = ImNodes::GetStyle();
    style.Colors[ImNodesCol_Link] = IM_COL32(204, 108, 27, 255);
    style.Colors[ImNodesCol_LinkSelected] = IM_COL32(241, 168, 26, 255);
    style.Colors[ImNodesCol_LinkHovered] = IM_COL32(232, 132, 27, 255);
    style.Colors[ImNodesCol_Pin] = style.Colors[ImNodesCol_Link];
    style.Colors[ImNodesCol_PinHovered] = style.Colors[ImNodesCol_LinkHovered];
    style.Colors[ImNodesCol_MiniMapLink] = style.Colors[ImNodesCol_Link];
    style.Colors[ImNodesCol_MiniMapLinkSelected] = style.Colors[ImNodesCol_LinkSelected];
    
    ImNodes::BeginNodeEditor();

    drawNodes_();
    drawLinks_();

    ImNodes::MiniMap(0.2, ImNodesMiniMapLocation_BottomRight);

    ImNodes::EndNodeEditor();

    testNewLink_();

    handleInput_();
}


void KcImNodeEditor::drawNodes_() const
{
    // 根据节点类型，配置r、g、b三个色系
    unsigned int clrTitleBar[] = {
        IM_COL32(191, 11, 109, 255),
        IM_COL32(11, 191, 109, 255),
        IM_COL32(11, 109, 191, 255)
    };

    unsigned int clrTitleBarSelected[] = {
        IM_COL32(204, 81, 148, 255),
        IM_COL32(81, 204, 148, 255),
        IM_COL32(81, 148, 204, 255)
    };

    unsigned int clrTitleBarHovered[] = {
        IM_COL32(194, 45, 126, 255),
        IM_COL32(45, 194, 126, 255),
        IM_COL32(45, 126, 194, 255)
    };

    for (unsigned v = 0; v < graph_.order(); v++) {

        auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
        assert(node); // block节点断言
        
        int nodeKind = node->inPorts() == 0 ? 0 : node->outPorts() == 0 ? 2 : 1;

        ImNodes::PushColorStyle(ImNodesCol_TitleBar, clrTitleBar[nodeKind]);
        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, clrTitleBarSelected[nodeKind]);
        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, clrTitleBarHovered[nodeKind]);

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

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
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


std::pair<int, int> KcImNodeEditor::nodeId_(int linkId)
{
    return { linkId >> 16, linkId & 0xff };
}


void KcImNodeEditor::testNewLink_()
{
    int fromId, toId;
    if (ImNodes::IsLinkCreated(&fromId, &toId)) {
        auto fromIdx = nodeId2Index_(fromId);
        auto toIdx = nodeId2Index_(toId);
        assert(fromIdx != -1 && toIdx != -1);

        auto node = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(fromIdx));
        assert(node && std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(toIdx)));
        if (node->type() != KcPortNode::k_in)
            std::swap(fromIdx, toIdx); // 用户反向建立link

        graph_.addEdge(fromIdx, toIdx);
    }
}


void KcImNodeEditor::handleInput_()
{
    // 响应delete按键，删除选中的节点或边

    auto numLinks = ImNodes::NumSelectedLinks();
    if (numLinks > 0 && ImGui::IsKeyReleased(ImGuiKey_Delete))
    {
        std::vector<int> links;
        links.resize(numLinks);
        ImNodes::GetSelectedLinks(links.data());
        for (auto linkId : links) {
            auto nodeIds = nodeId_(linkId);
            graph_.eraseEdge(nodeId2Index_(nodeIds.first), nodeId2Index_(nodeIds.second));
        }
    }

    auto numNodes = ImNodes::NumSelectedNodes();
    if (numNodes > 0 && ImGui::IsKeyReleased(ImGuiKey_Delete))
    {
        std::vector<int> nodes;
        nodes.resize(numNodes);
        ImNodes::GetSelectedNodes(nodes.data());
        for (auto nodeId : nodes) {
            auto nodeIdx = nodeId2Index_(nodeId);
            auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(nodeIdx));
            assert(node);

            unsigned v = nodeIdx + node->inPorts() + node->outPorts();
            for (; v > nodeIdx; v--)
                graph_.eraseVertex(v); // 删除port节点
            graph_.eraseVertex(nodeIdx); // 删除block节点
        }
    }
}
