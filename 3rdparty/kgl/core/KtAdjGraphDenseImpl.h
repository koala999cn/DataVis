#pragma once
#include "KtAdjGraphBase.h"
#include "../base/KtMatrix.h"
#include "graph_traits.h"
#include "edge_traits.h"
#include <assert.h>


// ���ڳ�������ڽӾ���ͼʵ��

template<typename EDGE_TYPE, typename VERTEX_TYPE = void>
class KtAdjGraphDenseImpl : public KtAdjGraphBase<KtMatrix<EDGE_TYPE>, VERTEX_TYPE>
{
public:
    using super_ = KtAdjGraphBase<KtMatrix<EDGE_TYPE>, VERTEX_TYPE>;
    using edge_type = EDGE_TYPE;
    using vertex_type = VERTEX_TYPE;
    using edge_iter = typename graph_traits<super_>::edge_iter;
    inline constexpr static const EDGE_TYPE& null_edge = edge_traits<EDGE_TYPE>::null_edge;

    using super_::E_;
    using super_::adjMat_;
    using super_::order;
    using super_::outedges;


    KtAdjGraphDenseImpl() = default;

    // ����ͼΪnv���㣬��������б�
    void reset(unsigned nv) {
        E_ = 0;
        adjMat_.resize(nv, nv, null_edge);
        if constexpr (!std::is_void_v<vertex_type>)
            super_::vertexes_.resize(nv);
    }

    // Ԥ��nv�������ne���ߵĴ洢.
    void reserve(unsigned nv, unsigned ne) { 
        adjMat_.reserve(nv, ne);
        if constexpr (!std::is_void_v<vertex_type>)
            super_::vertexes_.reserve(nv);
    }

    // �Զ���vԤ��ne���ߵĴ洢.
    void reserveEdges(unsigned v, unsigned ne) {}

    template<typename T = vertex_type, std::enable_if_t<std::is_void_v<T>, bool> = true>
    unsigned addVertex() {
        adjMat_.appendRow(null_edge);
        adjMat_.appendCol(null_edge);
        return static_cast<unsigned>(adjMat_.rows() - 1);
    }

    template<typename T, std::enable_if_t<!std::is_void_v<T>
        && std::is_convertible_v<T, vertex_type>, bool> = false>
    unsigned addVertex(const T& v) {
        adjMat_.appendRow(null_edge);
        adjMat_.appendCol(null_edge);
        super_::vertexes_.push_back(vertex_type(v));

        return static_cast<unsigned>(super_::vertexes_.size());
    }

    template<typename T, std::enable_if_t<!std::is_void_v<T>
        && std::is_same_v<T, vertex_type>, bool> = false>
    unsigned addVertex(T&& v) {
        adjMat_.appendRow(null_edge);
        adjMat_.appendCol(null_edge);
        super_::vertexes_.push_back(std::move(v));

        return static_cast<unsigned>(super_::vertexes_.size());
    }

        
    // @dummy: ��Ϊtrue�����ʾ��ӵ�������ͼ�ıߣ����β���������E_���ֲ���
    template<bool dummy = false>
    edge_iter addEdge(unsigned from, edge_iter pos, const edge_type& edge) {
        assert(from < order());
        assert(pos >= outedges(from).begin() && pos <= outedges(from).end());
        assert(*pos == null_edge);

        *pos = edge;
        if constexpr (!dummy) ++E_;
        return pos;
    }


    // ɾ������v
    // ���øú���ǰ��ȷ����ɾ��v��ӵ����б�
    void eraseVertex(unsigned v) {
        assert(v < order());
        assert(outedges(v).count(null_edge) == order());

        adjMat_.eraseRow(v), adjMat_.eraseCol(v);

        if constexpr (!std::is_void_v<vertex_type>)
            super_::vertexes_.erase(std::next(super_::vertexes_.begin(), v));
    }

    // ɾ������v�ĳ���e
    template<bool dummy = false>
    edge_iter eraseEdge(unsigned v, edge_iter pos) {
        assert(pos >= outedges(v).begin() && pos < outedges(v).end());
        assert(*pos != null_edge);
        *pos = null_edge;
        if constexpr (!dummy) --E_;
        return pos + 1;
    }


    template<bool dummy = false>
    edge_iter eraseEdges(unsigned v, edge_iter first, edge_iter last) {
        assert(first >= outedges(v).begin() && last <= outedges(v).end());

        for (; first != last; ++first)
            if (*first != null_edge) {
                *first = null_edge;
                if constexpr (!dummy) --E_;
            }

        return last;
    }
};


template<typename EDGE_TYPE, typename VERTEX_TYPE>
struct graph_traits<KtAdjGraphDenseImpl<EDGE_TYPE, VERTEX_TYPE>>
    : public graph_traits<KtAdjGraphBase<KtMatrix<EDGE_TYPE>, VERTEX_TYPE>>
{
    constexpr static bool reshapable = true;
    constexpr static bool immutable = false;
    constexpr static bool is_dense = true;
};
