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

    // 使用稀疏有向图存储node节点数据
    using node_graph = DigraphSx<bool, node_ptr>;

    explicit KcImNodeEditor(const std::string_view& name);

    //const char* type() const override { return "NodeEditor"; }

    int flags() const override;

    void insertNode(const std::shared_ptr<KvBlockNode>& node);

private:
    void updateImpl_() override;

    void drawNodes_() const;

    void drawLinks_() const;

    void testNewLink_(); // 检测是否有新连接

    unsigned nodeIndex_(const node_ptr& node) const;

    unsigned nodeId2Index_(int id) const;

    // node id pair --> link id
    static int linkId_(int fromId, int toId);

    // link id --> node id pair
    static std::pair<int, int> nodeId_(int linkId);

    void handleInput_();

private:
    node_graph graph_;
};
