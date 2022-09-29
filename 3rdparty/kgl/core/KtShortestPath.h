#pragma once
#include <vector>
#include "KtWeightor.h"
#include "KtBfsIter.h"
#include "KtDfsIter.h"
#include "KtTopologySort.h"


// ���·���㷨
// �����㷨�Ĺؼ��Ƕ��㴦���˳�򣬶�Ӧ��˳�������Dfs��Bfs��Pfs��Ts�ȷ�����
// ��ͬʵ�ֵ�ʱ�临�Ӷ����£�
// ��Դ��
//   Dijkstra�������ڷǸ�Ȩֵ��O(V^2)������ͼ����
//   PFS�������ڷǸ�Ȩֵ��O(E*lgV)�����ر߽�ֵ
//   TS���������޻�ͼ��O(E)������
//   Bellman-Ford���������޸���ͼ��O(V*E)���иĽ������
// ȫԴ��
//    Dijkstra�������ڷǸ�Ȩֵ��O(V^3)��������ͼ����ͬ
//    PFS�������ڷǸ�Ȩֵ��O(V*E*lgV)�����ر߽�ֵ
//    DFS���������޻�ͼ��O(V*E)��������ͼ����ͬ
//    Floyd���������޸���ͼ��O(V^3)��������ͼ����ͬ
//    Johnson���������޸���ͼ��O(V*E*lgV)�����ر߽�ֵ
// ע������Ȩֵ����ֵ��ƫ��ת������ͳһ��Johnson��δʵ��



// ��Դ���·���Ļ��ࣺ������ʼ����v0��Դ�㣩��ͼg����������������·��
// SSSP(Single Source Shortest Path)
template<typename GRAPH, class WEIGHTOR>
class KtSsspAbstract
{
    static_assert(GRAPH::isDigraph(), "KtSsspAbstract must be instantiated with Digraph.");

public:
    using weight_type = typename WEIGHTOR::weight_type;
    using vertex_index_t = typename GRAPH::vertex_index_t;

    KtSsspAbstract(const GRAPH& g, vertex_index_t v0) :
        v0_(v0),
        spt_(g.order(), -1),
        dist_(g.order(), WEIGHTOR{}.worst_weight) {}

    // ���ش�Դ�㵽����v�����·��(����)
    auto pathR(vertex_index_t v) const {
        std::vector<vertex_index_t> p;
        vertex_index_t s = v;
        do {
            p.push_back(s);
            s = spt_[s];
        } while (s != v0_ && s != v && s != -1);

        // s == -1����ʾû�л�·��·����ջ�ɹ�
        // s == v0����ʾv0���ڻ�·����Ҫ�ֶ���v0��ջ
        // s == v����ʾ���ھ���v�ĸ�������ʱ��ʧ�˴�v0->v�����·����ջ��·���˻�Ϊ��vΪ��ֹ��ĸ�����
        if(s != -1) p.push_back(s);

        return p;
    }


    // ���ش�Դ�㵽����v����̾���
    weight_type distance(vertex_index_t v) const {
        return dist_[v];
    }


    // �Ƿ���ڴ�Դ�㵽v��·��
    bool reachable(vertex_index_t v) const {
        return spt_[v] != -1;
    }


protected:

    // ���ɳ�. �ж�v0������(v, w)��w��·���Ƿ�ȵ�ǰ��w��·������
    // wtΪ��(v, w)��Ȩֵ
    bool relax_(vertex_index_t v, vertex_index_t w, weight_type wt) {
        if (spt_[v] != -1) {
            if (v != v0_)
                wt = WEIGHTOR{}.acc(wt, dist_[v]);
        }
        else if (v != v0_)
            return false;

        if(WEIGHTOR{}.comp(wt, dist_[w])) {
            dist_[w] = wt, spt_[w] = v;
            return true;
        }

        return false;
    }

protected:
    vertex_index_t v0_; // Դ��
    std::vector<vertex_index_t> spt_; // (spt_[i], i)��ʾ��Դ��v0_������i�����·���ϵ����һ����
    std::vector<weight_type> dist_; // dist_[i]��ʾ��Դ��v0_������i����̾���
};


// ���ڹ�����������ĵ�Դ���·��ʵ��
// ����E�ϴ���л�ͼʱ���ٶȷǳ���
// ����DAG�������ƣ�ò�Ʊ�TS��������
// NOTE: ���и�����ʱ�򣬻��������޵ݹ�
template<typename GRAPH, class WEIGHTOR = default_wtor<GRAPH>>
class KtSsspBfs : public KtSsspAbstract<GRAPH, WEIGHTOR>
{
    using super_ = KtSsspAbstract<GRAPH, WEIGHTOR>;
    using typename super_::vertex_index_t;

public:
    KtSsspBfs(const GRAPH& g, vertex_index_t v0) :super_(g, v0) {
        bfs_(g, v0);
    }

private:

    void bfs_(const GRAPH& g, vertex_index_t v) {
        KtBfsIter<const GRAPH, false, true> iter(g, v);
        for (; !iter.isEnd(); ++iter) {
            vertex_index_t v = iter.from(), w = *iter;

            // ���relax_�������ѳ�ջ�Ķ�����Ϣ������Ҫ�Գ�ջ��������bfs
            if (super_::relax_(v, w, WEIGHTOR{}(iter.edge())) && iter.isPopped(w))
                bfs_(g, w); 
        }
    }
};


// �ٶȱ�bfs���������ܶ࣬����
template<typename GRAPH, class WEIGHTOR = default_wtor<GRAPH>>
class KtSsspDfs : public KtSsspAbstract<GRAPH, WEIGHTOR>
{
    using super_ = KtSsspAbstract<GRAPH, WEIGHTOR>;
    using typename super_::vertex_index_t;

public:
    KtSsspDfs(const GRAPH& g, vertex_index_t v0) : super_(g, v0) {
        dfs_(g, v0);
    }

private:
    void dfs_(const GRAPH& g, vertex_index_t v) {
        KtDfsIter<const GRAPH, false, true> iter(g, v);
        for (; !iter.isEnd(); ++iter) {
            vertex_index_t v = iter.from(), w = *iter;

            if (super_::relax_(v, w, WEIGHTOR{}(iter.edge())))
                if (!iter.isTree() && w != v)
                    dfs_(g, w); // TODO: ���ֲ�ͬ���͵ıߣ���ߣ��±ߣ��رߣ�����һ���Ż�
        }
    }
};



// ����Dijkstra�㷨�ĵ�Դ���·��ʵ��
// NOTE: ���㷨��֧�ָ�����Ҳ��֧�ָ�Ȩֵ����Ϊһ�����ָ�Ȩֵ��
// ���ܻ����һ����֮ǰ���̵�·��������ƻ�ʼ���������·��������׼��
// Ҳ����˵�������޷��жϵ�ǰ�����·���ǲ��������̣���Ϊ������ֵĸ�Ȩֵ���ܻ�������·������
template<typename GRAPH, class WEIGHTOR = default_wtor<GRAPH>>
class KtSsspDijkstra : public KtSsspAbstract<GRAPH, WEIGHTOR>
{
    using super_ = KtSsspAbstract<GRAPH, WEIGHTOR>;
    using typename super_::vertex_index_t;
    using super_::spt_;
    using super_::dist_;

public:
    KtSsspDijkstra(const GRAPH& g, vertex_index_t v0) : super_(g, v0) {

        std::vector<bool> vis(g.order(), false); // ���ڱ��Դ��v0������i�����·���Ƿ��Ѽ���

        vertex_index_t v = v0;
        while (v != -1) { 
            vis[v] = true;

            // ���ɳ�
            for (vertex_index_t w = 0; w < g.order(); w++)
                if ((!vis[w] || w == v0/*permit loop path*/) && g.hasEdge(v, w))
                    super_::relax_(v, w, WEIGHTOR{}(g.getEdge(v, w)));


            // ��vis[i]����false�ļ����У�Ѱ�Ҿ���v0·�����ŵĶ���. TODO: ʹ�����ȶ���ʵ��
            v = -1;
            for (vertex_index_t w = 0; w < g.order(); w++)
                if (!vis[w] && (v == -1 || WEIGHTOR{}.comp(dist_[w], dist_[v])))
                    v = w;

            if (v == -1 || spt_[v] == -1/*disconnecte to v0*/) break; // all done
        }
    }
};



template<typename GRAPH, class WEIGHTOR = default_wtor<GRAPH>>
class KtSsspPfs : public KtSsspAbstract<GRAPH, WEIGHTOR>
{
    using super_ = KtSsspAbstract<GRAPH, WEIGHTOR>;
    using typename super_::vertex_index_t;
    using super_::dist_;

public:
    KtSsspPfs(const GRAPH& g, vertex_index_t v0) : super_(g, v0) {
        using element_type = std::pair<vertex_index_t, typename WEIGHTOR::weight_type>;
        struct Comp {
            bool operator()(const element_type& a, const element_type& b) {
                return WEIGHTOR{}.comp(b.second, a.second); // ���ȶ��еıȽϺ�����Ҫ�������Ƚϣ����ܱ�֤����Ԫ���ڶ��ж���
            }
        };

        using priority_queue = std::priority_queue<element_type, std::vector<element_type>, Comp>;
        priority_queue pq; pq.emplace(v0, dist_[v0]);
        std::vector<bool> vis(g.order(), false); // ���ڱ��Դ��v0������i�����·���Ƿ��Ѽ���

        while (!pq.empty()) {

            auto v = pq.top().first; pq.pop();
            if (vis[v]) continue;
            vis[v] = true;

            // ���ɳ�
            for (vertex_index_t w = 0; w < g.order(); w++)
                if ((!vis[w] || w == v0/*permit loop path*/) && g.hasEdge(v, w))
                    if (super_::relax_(v, w, WEIGHTOR{}(g.getEdge(v, w))))
                        pq.emplace(w, dist_[w]);
        }
    }
};


// �������������㷨�ĵ�Դ���·��ʵ�֣���������DAG.
// �����ٶ�û�������п죬Ч�ʻ��Ȳ���bfs����
template<typename DAG, class WEIGHTOR = default_wtor<DAG>>
class KtSsspTs : public KtSsspAbstract<DAG, WEIGHTOR>
{
    using super_ = KtSsspAbstract<DAG, WEIGHTOR>;
    using typename super_::vertex_index_t;

public:
    KtSsspTs(const DAG& g, vertex_index_t v0) : super_(g, v0) {
        assert(!has_loop(g));
        KtTopologySort<DAG> ts(g);
        auto j = ts.relabel(v0); // j֮ǰ�Ķ�����Ժ��ԣ���Ϊ������������v0������û�пɴ�·��
        for(auto v = ts[j++]; j < g.order(); v = ts[j++]) {
            auto iter = KtAdjIter(g, v);
            while(!iter.isEnd()) {
                super_::relax_(v, *iter, WEIGHTOR{}(iter.edge()));
                ++iter;
            }
        }
    }
};



// ����Bellman-Ford�㷨�ĵ�Դ���·��ʵ��
// NOTE: ���������������Ȩֵ�ĵ�Դ���·�����⣬ʱ�临�Ӷ�O(VE)��
// ���ڳ���ͼ��������ʱ�䲻��Floyd�㷨���ã�Floyd�㷨��Ҫ�ҳ�ȫ�����·�������������ǵ�Դ��
// ����ϡ��ͼ��������ʱ������Floyd�㷨��V�����������޸�Ȩֵ��ͼ��������ʱ���Dijkstra�㷨��ԼV����
template<typename GRAPH, class WEIGHTOR = default_wtor<GRAPH>>
class KtSsspBellmanFord : public KtSsspAbstract<GRAPH, WEIGHTOR>
{
    using super_ = KtSsspAbstract<GRAPH, WEIGHTOR>;
    using typename super_::vertex_index_t;

public:
    KtSsspBellmanFord(const GRAPH& g, vertex_index_t v0) : super_(g, v0) {
        std::queue<vertex_index_t> q;
        unsigned V = g.order();
        q.push(v0); q.push(V); // ���ֵV����ǰһ����������һ������ָ�����ʹ�ÿ�����V�鴦�����ֹ��
        unsigned N = 0;
        while (!q.empty()) {
            vertex_index_t v = q.front(); q.pop();
            while (v == V) {
                if (N++ > V) return; // ����һ�����ȳ���V�����·�������ʾ�϶����ڸ�����
                q.push(V);
                v = q.front(); q.pop();
            }

            auto iter = KtAdjIter(g, v);
            for (; !iter.isEnd(); ++iter)
                if (super_::relax_(v, *iter, WEIGHTOR{}(iter.edge()))
                    && *iter != v/*ignore self-loop*/)
                    q.push(*iter);
        }
    }
};



// ȫԴ���·���Ļ��ࣺ�������ⶥ�㵽ͼ����������������·��
// FSSP(Full Source Shortest Path)
template<typename GRAPH, class WEIGHTOR>
class KtFsspAbstract
{
    static_assert(GRAPH::isDigraph(), "KtFsspAbstract must be instantiated with Digraph.");

public:
    using weight_type = typename WEIGHTOR::weight_type;
    using vertex_index_t = typename GRAPH::vertex_index_t;

    KtFsspAbstract(const GRAPH& g) 
        : spt_(g.order(), std::vector<vertex_index_t>(g.order(), -1)),
        dst_(g.order(), std::vector<weight_type>(g.order(), WEIGHTOR{}.worst_weight)) {}


    weight_type distance(vertex_index_t v, vertex_index_t w) const {
        return dst_[v][w];
    }


    // ����v��w�����·��(����)
    auto pathR(vertex_index_t v, vertex_index_t w) const {
        std::vector<vertex_index_t> p;
        vertex_index_t s = w;
        do {
            p.push_back(s);
            s = spt_[v][s];
        } while (s != v && s != w && s != -1 );
        if (s != -1) p.push_back(s);

        return p;
    }

    // �Ƿ���ڴ�v��w��·��
    bool reachable(vertex_index_t v, vertex_index_t w) const {
        return spt_[v][w] != -1;
    }

protected:

    // ·���ɳ�. �ж�s����x�ٵ�t��·���Ƿ�ȵ�ǰs��t��·������
    bool relax_(vertex_index_t s, vertex_index_t t, vertex_index_t x) {
        assert(x != s && x != t); // s���ܵ���t�����ڼ�����̻�·
        const auto dst = WEIGHTOR{}.acc(dst_[s][x], dst_[x][t]);
        if (WEIGHTOR{}.comp(dst, dst_[s][t])) { // dst��dst_[s][t]����
            dst_[s][t] = dst;
            spt_[s][t] = spt_[x][t];
            return true;
        }

        return false;
    }


protected:

    template<typename T> using vector = std::vector<T>;
    vector<vector<vertex_index_t>> spt_;
    vector<vector<weight_type>> dst_;
};



// Floydʵ��ȫԴ���·��
// ʱ�临�Ӷ�O(V^3)�������ڳ���ͼ
// ����ϡ��ͼ�������������Dijkstra�㷨���㵥Դ���·�����Ӷ��õ�ȫԴ���·��
// NOTE: ���㷨δ��Ȩֵ���κμ��裬��˼�ʹ���ڸ�ȨֵҲ����Ч�ġ�
// ��������ڸ���������������·��������ֻ���ҳ�һ�����������������·����
template<typename GRAPH, class WEIGHTOR = default_wtor<GRAPH>>
class KtFsspFloyd : public KtFsspAbstract<GRAPH, WEIGHTOR>
{
    using super_ = KtFsspAbstract<GRAPH, WEIGHTOR>;
    using typename super_::vertex_index_t;
    using super_::spt_;
    using super_::dst_;

public:
    KtFsspFloyd(const GRAPH& g) : super_(g) {

        // ���߳�ʼ��spt_��dst_
        KtBfsIter<const GRAPH, true, true> iter(g, 0);
        for (; !iter.isEnd(); ++iter) {
            vertex_index_t v = iter.from(), w = *iter;
            spt_[v][w] = v;
            dst_[v][w] = WEIGHTOR{}(iter.edge());
        }

        const vertex_index_t V = g.order();

        // �ж�·��s->x->t�Ƿ��·��s->t����
        for (vertex_index_t x = 0; x < V; x++)
            for (vertex_index_t s = 0; s < V; s++)
                if (spt_[s][x] != -1 && s != x) // ��s->x��ͨ������s==x�����Թ�
                    for (vertex_index_t t = 0; t < V; t++)
                        if(t != x) super_::relax_(s, t, x);
    }
};



// DFSʵ��ȫԴ���·������������DAG
// ʱ�临�Ӷ�O(V*E)
// �ٶȱ�TS������ܶ�
template<typename GRAPH, class WEIGHTOR = default_wtor<GRAPH>>
class KtFsspDfs : public KtFsspAbstract<GRAPH, WEIGHTOR>
{
    using super_ = KtFsspAbstract<GRAPH, WEIGHTOR>;
    using typename super_::vertex_index_t;
    using super_::spt_;
    using super_::dst_;

public:
    KtFsspDfs(const GRAPH& g) : super_(g), done_(g.order(), false) {
        assert(!has_loop(g));

        for (vertex_index_t v = 0; v < g.order(); v++)
            if(!done_[v]) dfs_(g, v);
    }

private:
    void dfs_(const GRAPH& g, vertex_index_t v) {
        done_[v] = true;

        auto iter = KtAdjIter(g, v);
        for (; !iter.isEnd(); ++iter) {
            auto w = *iter;
            auto wt = WEIGHTOR{}(iter.edge());
            if (WEIGHTOR{}.comp(wt, dst_[v][w])) {
                dst_[v][w] = wt;
                spt_[v][w] = v;
            }

            if (!done_[w]) dfs_(g, w);

            if (w != v) {
                for (vertex_index_t i = 0; i < g.order(); i++)
                    if (spt_[w][i] != -1 && w != i)
                        super_::relax_(v, i, w);
            }
        }
    }

private:
    std::vector<bool> done_;
};

