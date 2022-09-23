#include "KcImNodeEditor.h"
#include "imnodes/imnodes.h"
#include "KvNode.h"
#include <assert.h>


KcImNodeEditor::KcImNodeEditor(const std::string_view& name)
    : KvImWindow(name)
{
    // test graph
    auto node = std::make_shared<KvNode>("test node", 2, 3);
    node->inName(0) = "in0"; node->inName(1) = "in1";
    node->outName(0) = "out0"; node->outName(1) = "out1"; node->outName(2) = "out2";
    graph_.addVertex(vertex_type{ node, -1 });

}


void KcImNodeEditor::draw()
{
    ImNodes::BeginNodeEditor();

    for (unsigned v = 0; v < graph_.order(); v++) {
     
        if (!isNode_(graph_.vertexAt(v)))
            continue; // 只轮询主节点，端口会在主节点绘制时考虑

        auto& node = graph_.vertexAt(v).first;

        ImNodes::BeginNode(node->id());

        ImNodes::BeginNodeTitleBar();
        const char* text = node->name().data();
        ImGui::TextUnformatted(text, text + node->name().size());
        ImNodes::EndNodeTitleBar();

        for (unsigned i = 0; i < node->ins(); i++) {
            ImNodes::BeginInputAttribute(node->inId(i));
            ImGui::Text(node->inName(i).c_str());
            ImNodes::EndInputAttribute();
        }

        ImGui::Spacing();

        for (unsigned i = 0; i < node->outs(); i++) {
            ImNodes::BeginOutputAttribute(node->outId(i));
            ImGui::Indent(40); // TODO
            ImGui::Text(node->outName(i).c_str());
            ImNodes::EndOutputAttribute();
        }

        ImNodes::EndNode();
    }


    ImNodes::EndNodeEditor();
}


bool KcImNodeEditor::isNode_(const vertex_type& v)
{
    return v.second == -1;
}


int KcImNodeEditor::getId_(const vertex_type& v) const
{
    if (isNode_(v)) { // 若是主节点，直接返回其id
        return v.first->id();
    }
    else { // 处理端口
        auto idx = vertex2Index_(v);
        if (graph_.outdegree(idx) > 0) { // 具有出边，表示其为输出端口
            assert(graph_.outdegree(idx) == 1 && graph_.indegree(idx) == 0);
            return v.first->outId(v.second);
        }
        else {
            assert(graph_.indegree(idx) == 1);
            return v.first->inId(v.second);
        }
    }
}


unsigned KcImNodeEditor::vertex2Index_(const vertex_type& v) const
{
    assert(false);
    return 0;
}