#pragma once
#include <vector>
#include <memory>
#include "KvNode.h"
#include "kgl/GraphX.h"

// 管线的结构与运行

class KgPipeline
{
public:
    using node_ptr = std::shared_ptr<typename KvNode::super_>;

    // 使用稀疏有向图存储node节点数据
    using node_graph = DigraphSx<bool, node_ptr>;


    /// 节点管理

    node_ptr getNode(int id) const;

    void insertNode(const std::shared_ptr<KvBlockNode>& node);

    void eraseNode(int nodeId);

    void insertLink(int fromId, int toId);

    void eraseLink(int fromId, int toId);

    const node_graph& graph() const { return graph_; }


    /// 管线管理

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

    // 返回端口节点的父节点的顶点序号
    // @v: 端口节点的顶点序号
    unsigned parentIndex_(unsigned v) const;

    unsigned nodeIndex_(const node_ptr& node) const;

    unsigned nodeId2Index_(int id) const;

private:
    node_graph graph_;

    unsigned frameIdx_{ 0 };
    int status_{ k_ready };
};