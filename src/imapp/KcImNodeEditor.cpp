#include "KcImNodeEditor.h"
#include "imnodes/imnodes.h"
#include <assert.h>
#include "kgl/core/KtBfsIter.h"
#include "kgl/util/inverse.h"


KcImNodeEditor::KcImNodeEditor(const std::string_view& name)
    : KvImWindow(name)
{
    minSize_[0] = 240, minSize_[1] = 120;
}


int KcImNodeEditor::flags() const
{
    return ImGuiWindowFlags_NoBringToFrontOnFocus; 
    // TODO: 同时设置ImGuiWindowFlags_DockNodeHost，在窗口首次获得焦点时会出现断言失败
}


void KcImNodeEditor::updateImpl_()
{
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

    // 在右下角显示缩略图
    ImNodes::MiniMap(0.2, ImNodesMiniMapLocation_BottomRight);

    bool focused = ImGui::IsWindowFocused();

    ImNodes::EndNodeEditor();

    if (focused)
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
        ImGui::BeginGroup();    // Inputs
        for (unsigned i = 0; i < node->inPorts(); i++) {
            auto& port = graph_.vertexAt(++v);

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
        ImGui::EndGroup();

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

    if (status_ == k_busy) {
        // TODO: node->onStartPipeline(); // 补一个流水线启动的回调
        
        // TODO: 检测错误
    }
}


void KcImNodeEditor::eraseNode(int nodeId)
{
    auto nodeIdx = nodeId2Index_(nodeId);
    assert(nodeIdx < graph_.order());
    auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(nodeIdx));
    assert(node);

    unsigned v = nodeIdx + node->inPorts() + node->outPorts();
    for (; v > nodeIdx; v--) {

        // 发送onDelLink通知
        auto port = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(v));
        if (port->type() == KcPortNode::k_out) { // 出边
            auto adj = KtAdjIter(graph_, v);
            for (; !adj.isEnd(); ++adj) {
                auto toNode = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(*adj));
                node->onDelLink(port.get(), toNode.get());
                toNode->parent().lock()->onDelLink(port.get(), toNode.get());
            }
        }
        else { // 入边
            auto ins = graph_.inedges(v);
            for (auto w : ins) {
                auto fromNode = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(w));
                node->onDelLink(fromNode.get(), port.get());
                fromNode->parent().lock()->onDelLink(fromNode.get(), port.get());
            }
        }

        graph_.eraseVertex(v); // 删除port节点
    }

    graph_.eraseVertex(nodeIdx); // 删除block节点
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
    // TODO: 可使用二分法搜索
    for (unsigned v = 0; v < graph_.order(); v++)
        if (id == graph_.vertexAt(v)->id())
            return v;

    return -1;
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


unsigned KcImNodeEditor::parentIndex_(unsigned v) const
{
    auto port = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(v));
    assert(port);

    if (port->type() == KcPortNode::k_in) {
        return v - port->index() - 1;
    }
    else {
        auto pnode = port->parent();
        return v - port->index() - 1 - pnode.lock()->inPorts();
    }
}


void KcImNodeEditor::handleInput_()
{
    int fromId, toId;
    if (ImNodes::IsLinkCreated(&fromId, &toId))
        insertLink(fromId, toId);


    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
        return;

    // 响应delete按键，删除选中的节点或边

    auto numLinks = ImNodes::NumSelectedLinks();
    if (numLinks > 0 && ImGui::IsKeyReleased(ImGuiKey_Delete)) {
        std::vector<int> links;
        links.resize(numLinks);
        ImNodes::GetSelectedLinks(links.data());
        for (auto linkId : links) {
            auto nodeIds = nodeId_(linkId);
            eraseLink(nodeIds.first, nodeIds.second);
        }
        ImNodes::ClearLinkSelection();
    }

    auto numNodes = ImNodes::NumSelectedNodes();
    if (numNodes > 0 && ImGui::IsKeyReleased(ImGuiKey_Delete)) {
        std::vector<int> nodes;
        nodes.resize(numNodes);
        ImNodes::GetSelectedNodes(nodes.data());
        for (auto nodeId : nodes)
            eraseNode(nodeId);
        ImNodes::ClearNodeSelection();
    }

    if (numNodes == 1 && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        int nodeId;
        ImNodes::GetSelectedNodes(&nodeId);
        auto node = graph_.vertexAt(nodeId2Index_(nodeId));
        auto block = std::dynamic_pointer_cast<KvBlockNode>(node);
        block->onDoubleClicked();
    }
}


void KcImNodeEditor::insertLink(int fromId, int toId)
{
    auto fromIdx = nodeId2Index_(fromId);
    auto toIdx = nodeId2Index_(toId);
    assert(fromIdx != -1 && toIdx != -1);

    auto fromNode = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(fromIdx));
    auto toNode = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(toIdx));
    assert(fromNode && toNode && fromNode->type() != toNode->type());
    if (fromNode->type() != KcPortNode::k_out) {
        std::swap(fromIdx, toIdx); // 确保边的起点始终是输出端口
        std::swap(fromNode, toNode);
    }

    if (fromNode->parent().lock()->onNewLink(fromNode.get(), toNode.get())
        && toNode->parent().lock()->onNewLink(fromNode.get(), toNode.get()))
        graph_.addEdge(fromIdx, toIdx);
}


void KcImNodeEditor::eraseLink(int fromId, int toId)
{
    auto fromIdx = nodeId2Index_(fromId);
    auto toIdx = nodeId2Index_(toId);
    assert(fromIdx != -1 && toIdx != -1);

    auto fromNode = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(fromIdx));
    auto toNode = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(toIdx));
    assert(fromNode && fromNode->type() == KcPortNode::k_out);
    assert(toNode && toNode->type() == KcPortNode::k_in);

    fromNode->parent().lock()->onDelLink(fromNode.get(), toNode.get());
    toNode->parent().lock()->onDelLink(fromNode.get(), toNode.get());
    graph_.eraseEdge(fromIdx, toIdx);
}


KcImNodeEditor::node_ptr KcImNodeEditor::getNode(int id) const
{
    auto v = nodeId2Index_(id);
    assert(v != -1); 
    return graph_.vertexAt(v);
}


KcImNodeEditor::node_ptr KcImNodeEditor::getSelectedNode() const
{
    auto numNodes = ImNodes::NumSelectedNodes();
    if (numNodes != 1)
        return nullptr;

    int nodeId;
    ImNodes::GetSelectedNodes(&nodeId);

    return getNode(nodeId);
}


bool KcImNodeEditor::start()
{
    if (graph_.isEmpty())
        return false;

    DigraphSx<bool> gR;
    inverse(graph_, gR); // 求解graph_的逆，方便快速获取各顶点的入边

    std::vector<std::pair<unsigned, KcPortNode*>> ins;
    for (unsigned v = 0; v < graph_.order(); v++) {
        auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
        assert(node);

        ins.clear();
        for (unsigned w = 0; w < node->inPorts(); w++) {
            auto adj = KtAdjIter(gR, v + w + 1); // gR的出边等于graph_的入边
            for (; !adj.isEnd(); ++adj) {
                auto port = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(*adj));
                assert(port);
                ins.emplace_back(w, port.get());
            }
        }

        if (!node->onStartPipeline(ins)) {
            stop();
            return false;
        }

        v += node->inPorts();
        v += node->outPorts();
    }

    frameIdx_ = 0;
    status_ = k_busy;

    return true;
}


void KcImNodeEditor::stop()
{
    for (unsigned v = 0; v < graph_.order(); v++) {
        auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
        assert(node);
        node->onStopPipeline();

        // 跳过port节点
        v += node->inPorts();
        v += node->outPorts();
    }

    status_ = k_ready;
}


void KcImNodeEditor::stepFrame()
{
    if (graph_.isEmpty())
        return;

    // 统计所有节点的入度
    std::vector<unsigned> indegs(graph_.order(), 0); 
    KtBfsIter<const node_graph, true, true> iter(graph_, 0);
    for (; !iter.isEnd(); ++iter) {
        assert(iter.from() != -1);
        ++indegs[*iter];
    }

    std::vector<unsigned> q; // 待处理的block节点队列

    // 初始化q为所有block节点
    for (unsigned v = 0; v < graph_.order(); v++) {
        auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
        assert(node);
        node->onNewFrame(frameIdx_);

        q.push_back(v);

        // 统计block节点的indegreee
        for (unsigned w = 1; w <= node->inPorts(); w++)
            indegs[v] += indegs[v + w];

        // 跳过inport节点
        v += node->inPorts();

        for (unsigned w = 1; w <= node->outPorts(); w++)
            assert(indegs[v + w] == 0);

        // 跳过outport节点
        v += node->outPorts();
    }

    bool deadloop = false; 
    while (!deadloop && !q.empty()) {
        deadloop = true;

        for (auto iter = q.begin(); iter != q.end(); ) {
            auto v = *iter;
            if (indegs[v] != 0) { 
                ++iter;
                continue;
            }

            // 该节点的所有输入均已fetch，可以output数据
            deadloop = false;
            iter = q.erase(iter);
            auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
            assert(node);
            node->output();
            for (unsigned portIdx = 0; portIdx < node->outPorts(); portIdx++) {
                auto w = v + node->inPorts() + portIdx + 1; // 输出端口的顶点序号
                auto outPort = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(w));
                assert(outPort && outPort->type() == KcPortNode::k_out);

                auto adj = KtAdjIter(graph_, w);
                for (; !adj.isEnd(); ++adj) { // 遍历第portIdx个输出端口的连接
                    auto inPort = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(*adj));
                    assert(inPort && inPort->type() == KcPortNode::k_in);
                    inPort->parent().lock()->onInput(outPort.get(), inPort->index());

                    --indegs[parentIndex_(*adj)];
                }

            }

        }
    }


    for (unsigned v = 0; v < graph_.order(); v++) {
        auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
        assert(node);
        node->onEndFrame(frameIdx_);

        // 跳过port节点
        v += node->inPorts();
        v += node->outPorts();
    }

    ++frameIdx_;
}