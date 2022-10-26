#pragma once
#include "KvImWindow.h"


class KcImNodeEditor : public KvImWindow
{
public:

    explicit KcImNodeEditor(const std::string_view& name);

    int flags() const override;

    // ����-1��ʾ��ѡ�нڵ�����ж��ѡ�нڵ�
    int getSelectedNodeId() const;

private:
    void updateImpl_() override;

    void drawNodes_() const;

    void drawLinks_() const;

    // node id pair --> link id
    static int linkId_(int fromId, int toId);

    // link id --> node id pair
    static std::pair<int, int> nodeId_(int linkId);

    void handleInput_();
};
