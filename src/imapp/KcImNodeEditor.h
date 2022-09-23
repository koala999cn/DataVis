#pragma once
#include "KvImWindow.h"
#include "kgl/GraphX.h"
#include <memory>
#include <vector>

class KvNode;

class KcImNodeEditor : public KvImWindow
{
public:
    using node_ptr = std::shared_ptr<KvNode>;
    using vertex_type = std::pair<node_ptr, unsigned>; // 节点node的第i个端口

    // 使用稀疏有向图存储node节点数据
    using node_graph = DigraphSx<bool, vertex_type>;

    explicit KcImNodeEditor(const std::string_view& name);

    const char* id() const override { return "NodeEditor"; }

    void draw() override;

private:
    static bool isNode_(const vertex_type& v); // 判断顶点v是node还是port
    int getId_(const vertex_type& v) const;
    unsigned vertex2Index_(const vertex_type& v) const;

private:
    node_graph graph_;
};
