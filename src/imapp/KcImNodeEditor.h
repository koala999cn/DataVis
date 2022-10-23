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

    // ʹ��ϡ������ͼ�洢node�ڵ�����
    using node_graph = DigraphSx<bool, node_ptr>;

    explicit KcImNodeEditor(const std::string_view& name);

    int flags() const override;

    void insertNode(const std::shared_ptr<KvBlockNode>& node);

    void eraseNode(int nodeId);

    void insertLink(int fromId, int toId);

    void eraseLink(int fromId, int toId);

    node_ptr getNode(int id) const;

    node_ptr getSelectedNode() const;

    // pipeline֧��

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

private:
    void updateImpl_() override;

    void drawNodes_() const;

    void drawLinks_() const;

    unsigned nodeIndex_(const node_ptr& node) const;

    unsigned nodeId2Index_(int id) const;

    // node id pair --> link id
    static int linkId_(int fromId, int toId);

    // link id --> node id pair
    static std::pair<int, int> nodeId_(int linkId);

    // ���ض˿ڽڵ�ĸ��ڵ�Ķ������
    // @v: �˿ڽڵ�Ķ������
    unsigned parentIndex_(unsigned v) const;

    void handleInput_();

private:
    node_graph graph_;

    unsigned frameIdx_{ 0 };
    int status_{ k_ready };
};
