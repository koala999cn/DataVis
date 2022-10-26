#pragma once
#include <vector>
#include <memory>
#include "KvNode.h"
#include "kgl/GraphX.h"

// ���ߵĽṹ������

class KgPipeline
{
public:
    using node_ptr = std::shared_ptr<typename KvNode::super_>;

    // ʹ��ϡ������ͼ�洢node�ڵ�����
    using node_graph = DigraphSx<bool, node_ptr>;


    /// �ڵ����

    node_ptr getNode(int id) const;

    void insertNode(const std::shared_ptr<KvBlockNode>& node);

    void eraseNode(int nodeId);

    void insertLink(int fromId, int toId);

    void eraseLink(int fromId, int toId);

    const node_graph& graph() const { return graph_; }


    /// ���߹���

    enum KeStatus
    {
        k_ready,
        k_busy,
        k_paused
    };

    bool start();

    void stop();

    void stepFrame();

    int status() const { return status_; }

    bool running() const {
        return status_ == k_busy;
    }


private:

    // ���ض˿ڽڵ�ĸ��ڵ�Ķ������
    // @v: �˿ڽڵ�Ķ������
    unsigned parentIndex_(unsigned v) const;

    unsigned nodeIndex_(const node_ptr& node) const;

    unsigned nodeId2Index_(int id) const;

private:
    node_graph graph_;

    unsigned frameIdx_{ 0 };
    int status_{ k_ready };
};