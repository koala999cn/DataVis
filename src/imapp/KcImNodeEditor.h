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
    using vertex_type = std::pair<node_ptr, unsigned>; // �ڵ�node�ĵ�i���˿�

    // ʹ��ϡ������ͼ�洢node�ڵ�����
    using node_graph = DigraphSx<bool, vertex_type>;

    explicit KcImNodeEditor(const std::string_view& name);

    const char* id() const override { return "NodeEditor"; }

    void draw() override;

private:
    static bool isNode_(const vertex_type& v); // �ж϶���v��node����port
    int getId_(const vertex_type& v) const;
    unsigned vertex2Index_(const vertex_type& v) const;

private:
    node_graph graph_;
};
