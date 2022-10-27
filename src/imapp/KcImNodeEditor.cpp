#include "KcImNodeEditor.h"
#include "imnodes/imnodes.h"
#include "KsImApp.h"
#include "KgPipeline.h"
#include <assert.h>


KcImNodeEditor::KcImNodeEditor(const std::string_view& name)
    : KvImWindow(name)
{
    minSize_[0] = 240, minSize_[1] = 120;
}


int KcImNodeEditor::flags() const
{
    return ImGuiWindowFlags_NoBringToFrontOnFocus; 
    // TODO: ͬʱ����ImGuiWindowFlags_DockNodeHost���ڴ����״λ�ý���ʱ����ֶ���ʧ��
}


void KcImNodeEditor::updateImpl_()
{
    bool disable = KsImApp::singleton().pipeline().running();
    ImGui::BeginDisabled(disable);

    // set the titlebar color for all nodes
    ImNodesStyle& style = ImNodes::GetStyle();
    style.Colors[ImNodesCol_Link] = IM_COL32(204, 108, 27, 255);
    style.Colors[ImNodesCol_LinkSelected] = IM_COL32(241, 198, 56, 255);
    style.Colors[ImNodesCol_LinkHovered] = IM_COL32(232, 165, 35, 255);
    style.Colors[ImNodesCol_Pin] = style.Colors[ImNodesCol_Link];
    style.Colors[ImNodesCol_PinHovered] = style.Colors[ImNodesCol_LinkHovered];
    style.Colors[ImNodesCol_MiniMapLink] = style.Colors[ImNodesCol_Link];
    style.Colors[ImNodesCol_MiniMapLinkSelected] = style.Colors[ImNodesCol_LinkSelected];
    
    ImNodes::BeginNodeEditor();

    drawNodes_();
    drawLinks_();

    // �����½���ʾ����ͼ
    ImNodes::MiniMap(0.2, ImNodesMiniMapLocation_BottomRight);

    bool focused = ImGui::IsWindowFocused();

    ImNodes::EndNodeEditor();

    if (focused)
        handleInput_();

    ImGui::EndDisabled();
}


void KcImNodeEditor::drawNodes_() const
{
    // ���ݽڵ����ͣ�����r��g��b����ɫϵ
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
  
    auto& graph = KsImApp::singleton().pipeline().graph();
    for (unsigned v = 0; v < graph.order(); v++) {

        auto node = std::dynamic_pointer_cast<KvBlockNode>(graph.vertexAt(v));
        assert(node); // block�ڵ����
        
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

        // �˿ڽڵ��index��Ȼ��bolck�ڵ�����
        ImGui::BeginGroup();    // Inputs
        for (unsigned i = 0; i < node->inPorts(); i++) {
            auto& port = graph.vertexAt(++v);

            auto thisWidth = ImGui::CalcTextSize(port->name().c_str()).x;
            if (thisWidth > maxInputWidth)
                maxInputWidth = thisWidth;

            ImNodes::BeginInputAttribute(port->id());
            ImGui::Text(port->name().c_str());
            ImNodes::EndInputAttribute();
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        if (node->inPorts() > 0)
            ImGui::Spacing();

        ImGui::BeginGroup();    // Outputs
        for (unsigned i = 0; i < node->outPorts(); i++) {
            auto& port = graph.vertexAt(++v);
            ImNodes::BeginOutputAttribute(port->id());

            auto thisWidth = ImGui::CalcTextSize(port->name().c_str()).x;

            if (thisWidth < titleWidth)
                ImGui::Indent(titleWidth - thisWidth);
            else
                ImGui::Indent(maxInputWidth + 2);

            ImGui::Text(port->name().c_str());
            ImNodes::EndOutputAttribute();
        }
        ImGui::EndGroup();

        ImNodes::EndNode();

        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
    }
}


void KcImNodeEditor::drawLinks_() const
{
    auto& graph = KsImApp::singleton().pipeline().graph();

    if (graph.size() > 0) {
        KtBfsIter<typename const KgPipeline::node_graph, true, true> iter(graph, 0);
        for (; !iter.isEnd(); ++iter) {
            auto& from = graph.vertexAt(iter.from());
            auto& to = graph.vertexAt(*iter);
            ImNodes::Link(linkId_(from->id(), to->id()), from->id(), to->id());
        }
    }
}



int KcImNodeEditor::linkId_(int fromId, int toId)
{
    assert(fromId < std::numeric_limits<short>::max() 
        && toId < std::numeric_limits<short>::max());

    return fromId << 16 | toId;
}


std::pair<int, int> KcImNodeEditor::nodeId_(int linkId)
{
    return { linkId >> 16, linkId & 0xff };
}


void KcImNodeEditor::handleInput_()
{
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
        return;

    auto& pipe = KsImApp::singleton().pipeline();

    // ����start/stop�Ŀ�ݼ�
    if (ImGui::IsKeyReleased(ImGuiKey_Enter)) {
        if (pipe.running())
            pipe.stop();
        else
            pipe.start();
    }


    // �½�����
    int fromId, toId;
    if (!pipe.running() && ImNodes::IsLinkCreated(&fromId, &toId))
        pipe.insertLink(fromId, toId);

    // ��Ӧdelete������ɾ��ѡ�еĽڵ���

    auto numLinks = ImNodes::NumSelectedLinks();
    if (!pipe.running() && numLinks > 0 && ImGui::IsKeyReleased(ImGuiKey_Delete)) {
        std::vector<int> links;
        links.resize(numLinks);
        ImNodes::GetSelectedLinks(links.data());
        for (auto linkId : links) {
            auto nodeIds = nodeId_(linkId);
            pipe.eraseLink(nodeIds.first, nodeIds.second);
        }
        ImNodes::ClearLinkSelection();
    }

    auto numNodes = ImNodes::NumSelectedNodes();
    if (!pipe.running() && numNodes > 0 && ImGui::IsKeyReleased(ImGuiKey_Delete)) {
        std::vector<int> nodes;
        nodes.resize(numNodes);
        ImNodes::GetSelectedNodes(nodes.data());
        for (auto nodeId : nodes)
            pipe.eraseNode(nodeId);
        ImNodes::ClearNodeSelection();
    }

    // ��Ӧ������˫��������ѡ��node��onDoubleClicked�ӿڷ���
    if (numNodes == 1 && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        int nodeId;
        ImNodes::GetSelectedNodes(&nodeId);
        auto node = pipe.getNode(nodeId);
        auto block = std::dynamic_pointer_cast<KvBlockNode>(node);
        block->onDoubleClicked();
    }
}


int KcImNodeEditor::getSelectedNodeId() const
{
    auto numNodes = ImNodes::NumSelectedNodes();
    if (numNodes != 1)
        return -1;

    int nodeId;
    ImNodes::GetSelectedNodes(&nodeId);

    return nodeId;
}
