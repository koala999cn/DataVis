#include "KgPipeline.h"
#include "kgl/core/KtBfsIter.h"
#include "kgl/util/inverse.h"


unsigned KgPipeline::nodeIndex_(const node_ptr& node) const
{
    for (unsigned v = 0; v < graph_.order(); v++)
        if (node == graph_.vertexAt(v))
            return v;

    return -1;
}


unsigned KgPipeline::nodeId2Index_(int id) const
{
    // TODO: ��ʹ�ö��ַ�����
    for (unsigned v = 0; v < graph_.order(); v++)
        if (id == graph_.vertexAt(v)->id())
            return v;

    return -1;
}


unsigned KgPipeline::parentIndex_(unsigned v) const
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


KgPipeline::node_ptr KgPipeline::getNode(int id) const
{
    auto v = nodeId2Index_(id);
    assert(v != -1);
    return graph_.vertexAt(v);
}


void KgPipeline::insertNode(const std::shared_ptr<KvBlockNode>& node)
{
    graph_.addVertex(node);

    // ��������˿ڽڵ�
    for (unsigned i = 0; i < node->inPorts(); i++)
        graph_.addVertex(std::make_shared<KcPortNode>(KcPortNode::k_in, node, i));


    // ��������˿ڽڵ�
    for (unsigned i = 0; i < node->outPorts(); i++)
        graph_.addVertex(std::make_shared<KcPortNode>(KcPortNode::k_out, node, i));

    if (status_ == k_busy) {
        // TODO: node->onStartPipeline(); // ��һ����ˮ�������Ļص�

        // TODO: ������
    }
}


void KgPipeline::eraseNode(int nodeId)
{
    auto nodeIdx = nodeId2Index_(nodeId);
    assert(nodeIdx < graph_.order());
    auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(nodeIdx));
    assert(node);

    unsigned v = nodeIdx + node->inPorts() + node->outPorts();
    for (; v > nodeIdx; v--) {

        // ����onDelLink֪ͨ
        auto port = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(v));
        if (port->type() == KcPortNode::k_out) { // ����
            auto adj = KtAdjIter(graph_, v);
            for (; !adj.isEnd(); ++adj) {
                auto toNode = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(*adj));
                node->onDelLink(port.get(), toNode.get());
                toNode->parent().lock()->onDelLink(port.get(), toNode.get());
            }
        }
        else { // ���
            auto ins = graph_.inedges(v);
            for (auto w : ins) {
                auto fromNode = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(w));
                node->onDelLink(fromNode.get(), port.get());
                fromNode->parent().lock()->onDelLink(fromNode.get(), port.get());
            }
        }

        graph_.eraseVertex(v); // ɾ��port�ڵ�
    }

    graph_.eraseVertex(nodeIdx); // ɾ��block�ڵ�
}



void KgPipeline::insertLink(int fromId, int toId)
{
    auto fromIdx = nodeId2Index_(fromId);
    auto toIdx = nodeId2Index_(toId);
    assert(fromIdx != -1 && toIdx != -1);

    auto fromNode = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(fromIdx));
    auto toNode = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(toIdx));
    assert(fromNode && toNode && fromNode->type() != toNode->type());
    if (fromNode->type() != KcPortNode::k_out) {
        std::swap(fromIdx, toIdx); // ȷ���ߵ����ʼ��������˿�
        std::swap(fromNode, toNode);
    }

    if (fromNode->parent().lock()->onNewLink(fromNode.get(), toNode.get())
        && toNode->parent().lock()->onNewLink(fromNode.get(), toNode.get()))
        graph_.addEdge(fromIdx, toIdx);
}


void KgPipeline::eraseLink(int fromId, int toId)
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


bool KgPipeline::start()
{
    if (graph_.isEmpty())
        return false;

    DigraphSx<bool> gR;
    inverse(graph_, gR); // ���graph_���棬������ٻ�ȡ����������

    std::vector<std::pair<unsigned, KcPortNode*>> ins;
    for (unsigned v = 0; v < graph_.order(); v++) {
        auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
        assert(node);

        ins.clear();
        for (unsigned w = 0; w < node->inPorts(); w++) {
            auto adj = KtAdjIter(gR, v + w + 1); // gR�ĳ��ߵ���graph_�����
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


void KgPipeline::stop()
{
    for (unsigned v = 0; v < graph_.order(); v++) {
        auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
        assert(node);
        node->onStopPipeline();

        // ����port�ڵ�
        v += node->inPorts();
        v += node->outPorts();
    }

    status_ = k_ready;
}


void KgPipeline::stepFrame()
{
    if (graph_.isEmpty())
        return;

    // ͳ�����нڵ�����
    std::vector<unsigned> indegs(graph_.order(), 0);
    KtBfsIter<const node_graph, true, true> iter(graph_, 0);
    for (; !iter.isEnd(); ++iter) {
        assert(iter.from() != -1);
        ++indegs[*iter];
    }

    std::vector<unsigned> q; // ��������block�ڵ����

    // ��ʼ��qΪ����block�ڵ�
    for (unsigned v = 0; v < graph_.order(); v++) {
        auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
        assert(node);
        node->onNewFrame(frameIdx_);

        q.push_back(v);

        // ͳ��block�ڵ��indegreee
        for (unsigned w = 1; w <= node->inPorts(); w++)
            indegs[v] += indegs[v + w];

        // ����inport�ڵ�
        v += node->inPorts();

        for (unsigned w = 1; w <= node->outPorts(); w++)
            assert(indegs[v + w] == 0);

        // ����outport�ڵ�
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

            // �ýڵ�������������fetch������output����
            deadloop = false;
            iter = q.erase(iter);
            auto node = std::dynamic_pointer_cast<KvBlockNode>(graph_.vertexAt(v));
            assert(node);
            node->output();
            for (unsigned portIdx = 0; portIdx < node->outPorts(); portIdx++) {
                auto w = v + node->inPorts() + portIdx + 1; // ����˿ڵĶ������
                auto outPort = std::dynamic_pointer_cast<KcPortNode>(graph_.vertexAt(w));
                assert(outPort && outPort->type() == KcPortNode::k_out);

                auto adj = KtAdjIter(graph_, w);
                for (; !adj.isEnd(); ++adj) { // ������portIdx������˿ڵ�����
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

        // ����port�ڵ�
        v += node->inPorts();
        v += node->outPorts();
    }

    ++frameIdx_;
}