#pragma once
#include "KvImWindow.h"
#include "kgl/GraphX.h"
#include <memory>
#include <vector>
#include "KvNode.h"


class KcImNodeEditor : public KvImWindow
{
public:
    using node_ptr = std::shared_ptr<typename KvNode::super_>;
    using vertex_type = node_ptr;

    // 使用稀疏有向图存储node节点数据
    using node_graph = DigraphSx<bool, vertex_type>;

    explicit KcImNodeEditor(const std::string_view& name);

    const char* type() const override { return "NodeEditor"; }

    void update() override;

private:
    unsigned vertex2Index_(const vertex_type& v) const;

private:
    node_graph graph_;
};
