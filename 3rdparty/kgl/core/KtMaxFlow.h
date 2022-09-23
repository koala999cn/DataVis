#pragma once
#include <vector>
#include <assert.h>
#include "KtWeightor.h"
#include "KtShortestPath.h"
#include "KtBfsIter.h"
#include "../util/copy.h"
#include "../GraphX.h"


// 最大流：给定一个st-网，找出一个流，从而使得从s到t的任何其他流都不会有更大的流值。
// 主要有两大类算法：一是扩充路径最大流算法，二是预流-压入最大流算法

// 以上两大类算法均依赖于【余留网（residual network）】：给定一个流网络和一个流，流的余留网有着原网相同的顶点，
// 对于原网中的各条边，余留网中将有1条或2条边，有如下定义：对于原网中的各条边v-w，
//    令f为流量，c为容量， 那么：
//    ①如果f != c，则在余留网中包括一条容量为c-f的前边v-w；（代表余留量，即该条边还可扩充或压入的流量）
//    ②如果f != 0，则在余留网中包括一条容量为f的回边w-v。（代表当前量，即该条边已有的流量，也即可回退的流量）


// 最大流的基类，实现余留网基本操作
template<typename GRAPH, typename WEIGHTOR>
class KtMaxFlow
{
protected:
    using flow_type = typename WEIGHTOR::weight_type;
    using RGRAPH = DigraphSx<flow_type>; // 余留网类型

    static_assert(std::is_integral_v<flow_type>, "max flow algorithm only support integral weight");

    // 对输入g的限制：
    //   1. g为st网，即有且仅有1个源点、1个汇点；
    //   2. 若v-w为g的边，则w-v不能是g的边。
    KtMaxFlow(const GRAPH& g, unsigned s, unsigned t)
        : graph_(g), // 保存原图引用
          rg_(copy<GRAPH, RGRAPH, WEIGHTOR>(g)) { // 构造余留网
        assert(s != t);
    }


public:

    // 返回边v-w的网容量
    auto cap(unsigned v, unsigned w) const {
        return WEIGHTOR{}(graph_.getEdge(v, w));
    }


    // 返回边v-w的网流量
    auto flow(unsigned v, unsigned w) const {
        assert(graph_.hasEdge(v, w));
        return rg_.hasEdge(w, v) ? rg_.getEdge(w, v) : 0;
    }


    // 返回边v-w的余留量 := cap - flow
    auto residual(unsigned v, unsigned w) const {
        assert(graph_.hasEdge(v, w));
        return rg_.hasEdge(v, w) ? rg_.getEdge(v, w) : 0;
    }


    // 返回顶点v的流出量
    auto outflow(unsigned v) const {
        flow_type f(0);

        auto iter = KtAdjIter(graph_, v);
        for (; !iter.isEnd(); ++iter)
            f += flow(v, *iter);

        return f;
    }


    // 返回顶点v的流入量
    auto inflow(unsigned v) const {
        flow_type f(0);

        for (unsigned w = 0; w < graph_.order(); w++)
            if (graph_.hasEdge(w, v))
                f += flow(w, v);

        return f;
    }


    // 返回顶点v的净流量 = 流入量 - 流出量 
    auto netflow(unsigned v) const {
        return inflow(v) - outflow(v);
    }


    bool check(unsigned s, unsigned t) const {
        if (outflow(s) != inflow(t))
            return false;

        for (unsigned v = 0; v < graph_.order(); v++)
            if (v != s && v != t && netflow(v) != 0)
                return false;

        return true;
    }


protected:

    // 对余留网的边v-w增加网流delta
    void addFlow_(unsigned v, unsigned w, flow_type delta) {
        assert(rg_.hasEdge(v, w));
        flow_type f = rg_.getEdge(v, w);
        f -= delta; // 该条边的可增广流量减少delta
        if (f == 0)
            rg_.eraseEdge(v, w);
        else
            rg_.setEdge(v, w, f);

        // 对应回边的可增广流量增加delta
        if (rg_.hasEdge(w, v))
            rg_.setEdge(w, v, rg_.getEdge(w, v) + delta);
        else
            rg_.addEdge(w, v, delta);
    }


protected:
    const GRAPH& graph_; // 原图
    RGRAPH rg_; // 余留网
};



// 扩充路径最大流算法的基类
// 扩充路径最大流算法也称Ford-Fulkerson算法
// 这是一种沿着从源点到汇点的路径逐渐增加流的通用方法
// 对于任意无满的前边且无空的回边的路径，可以通过增加前边中的流，同时减少回边中的流来增加网中的流量。
// 可以增加的流量由回边中的流和前边中未用容量的最小值所限制。
// 在新的流中，至少路径上有一条前边被充满，或者有一条回边变为空。

// 简而言之，余留网的边权值就是可增广的流量值，在余留网中搜索s到t的路径，即为增广路径。

// 增广路径搜索算法常见有2种：
//   一是最大容量扩充路径，即沿着可以使流得到最大增长的路径扩充。
//   二是最短扩充路径，即即沿着边数最少的路径扩充。

// Ford-Fulkerson算法任何实现所需的扩充路径数目最多等于V*M,
// 找出一个最大流所需时间为O(V*E*M)，对于稀疏网则为O(V^2*M)。其中，M为网中最大的边容量。
template<typename GRAPH, class WEIGHTOR>
class KtPathAugment : public KtMaxFlow<GRAPH, WEIGHTOR>
{
protected:
    using super_ = KtMaxFlow<GRAPH, WEIGHTOR>;
    using super_::super_; // 导入构造函数
    using super_::rg_;
    using super_::addFlow_;
    using typename super_::flow_type;
    using typename super_::RGRAPH;


protected:

    // 返回余留网逆路径pathR的可增广网络流：路径上的最小边
    flow_type minFlow_(const std::vector<unsigned>& pathR) const {
        auto iter = pathR.begin();
        unsigned w = *iter++;
        unsigned v = *iter++;
        flow_type d = rg_.getEdge(v, w);

        while (iter != pathR.end()) {
            w = v;
            v = *iter++;
            flow_type x = rg_.getEdge(v, w);
            if (x < d) d = x;
        }

        return d;
    }


    // 对逆路径pathR进行增广
    void augment_(const std::vector<unsigned>& pathR, flow_type delta) {
        // 沿着路径增广delta
        auto iter = pathR.begin();
        auto w = *iter++;
        unsigned v;
        for (; iter != pathR.end(); w = v, ++iter) {
            v = *iter;
            addFlow_(v, w, delta);
        }
    }
};



// 基于优先队列搜索的最大容量扩充路径算法
// 该算法所需的扩充路径数目最多等于2*E*lgM，找出稀疏网中一个最大流所需时间为O(V^2*lgM*lgV)。
// 对许多实际情况，最大容量扩充路径算法所用的迭代远远小于最短扩充路径算法所用的迭代，
// 不过对于找到各条路径的工作则需要一个稍高的开销。
template<typename GRAPH, class WEIGHTOR = default_wtor<GRAPH>>
class KtMaxFlowPfs : public KtPathAugment<GRAPH, WEIGHTOR>
{
public:
    using super_ = KtPathAugment<GRAPH, WEIGHTOR>;
    using super_::rg_;
    using super_::augment_;
    using super_::minFlow_;    
    using typename super_::flow_type;
    using typename super_::RGRAPH;


    KtMaxFlowPfs(const GRAPH& g, unsigned s, unsigned t) : super_(g, s, t) {
        
        // 可增广流量为路径上的最小值
        struct adder {
            auto operator()(const flow_type& f1, const flow_type& f2) const {
                return std::min(f1, f2);
            }
        };

        // 取可增广流量最大的路径
        using weightor = KtWeightorMax<KtWeightSelf<flow_type>, adder>;

        while (true) {
            KtSsspPfs<RGRAPH, weightor> pfs(rg_, s);
            auto path = pfs.pathR(t);
            if (path.size() < 2)
                break;
            assert(path.back() == s && path.front() == t);

            // 增广路径，更新余留网的权值
            assert(pfs.distance(t) == minFlow_(path));
            augment_(path, pfs.distance(t));
        }
    }
};



// 基于广度优先搜索的最短扩充路径算法，也称Edmonds-Karp算法
// 该算法所需的扩充路径数目最多等于V*E/2，找出稀疏网中一个最大流所需时间为O(V^3)。
template<typename GRAPH, class WEIGHTOR = default_wtor<GRAPH>>
class KtMaxFlowBfs : public KtPathAugment<GRAPH, WEIGHTOR>
{
public:
    using super_ = KtPathAugment<GRAPH, WEIGHTOR>;
    using super_::rg_;
    using super_::augment_;
    using super_::minFlow_;
    using typename super_::flow_type;
    using typename super_::RGRAPH;


    KtMaxFlowBfs(const GRAPH& g, unsigned s, unsigned t) : super_(g, s, t) {

        // 取边数最少的路径
        using weightor = KtWeightorMin<KtWeightUnit<flow_type>, KtAdder<typename KtWeightUnit<flow_type>::weight_type>>;

        while (true) {
            KtSsspPfs<RGRAPH, weightor> bfs(rg_, s);
            auto path = bfs.pathR(t);
            if (path.size() < 2)
                break;
            assert(path.back() == s && path.front() == t);

            // 增广路径，更新余留网的权值
            augment_(path, minFlow_(path));
        }
    }
};



// 预流-压入最大流算法
// 【预流(preflow)】流网络中满足以下条件的正边流的集合，即各条边上的流小于等于该边容量，
//    而且对于每个内部顶点，流入量大于等于流出量。
// 【活动顶点(active vertex)】流入量大于流出量的内部顶点（约定源点和汇点是不活动的）。
// 【超额量(excess flow)】活动顶点的流入量与流出量之差。
// 【顶点高度】每个顶点被赋予一个高度值，其中：汇点h(t) = 0，余留网中的每条边u-v均有h(u) <= h(v) + 1。
// 【合格边】余留网中满足h(u) = h(v) + 1的一条边u-v。

// 算法框架如下（基于边）：
// -- 初始化：从任意高度函数开始，并且除了连接到源点的边充满至其容量外，所有其他边为0流。
// -- 循环以下步骤，直至没有活动顶点为止：
//    1. 选择一个活动顶点；
//    2. 对离开该顶点的合格边（如果有的话）压入流；
//    3. 如果没有合格边，则增加顶点的高度。
// 算法时间复杂度为O(V^2*E)
// 以上算法并没有指定高度函数是什么、如何选择活动顶点、如何选择合格边、每次压入多少流等等，
// 通过对以上选项的不同选择，可得到不同的算法实现。

// 基于顶点的算法选择：
//    1. 选择一个活动顶点后，持续选择它的合格边，直至该顶点变为非活动或再无合格边为止。
//    2. 每次尽可能压入多的流量。

// KtMaxFlowPre是一个基于顶点算法的实现，并进一步具体化选项：
//    1. 使用FIFO队列依序选择活动顶点。
//    2. 源点高度初始化为V，其他为0。

// TODO: 1. 使用基于高度值的优先队列代替FIFO队列，能获得更好性能。
//       2. 更泛化的代码实现。 
template<typename GRAPH, class WEIGHTOR = default_wtor<GRAPH>>
class KtMaxFlowPre : public KtMaxFlow<GRAPH, WEIGHTOR>
{
public:
    using super_ = KtMaxFlow<GRAPH, WEIGHTOR>;
    using super_::rg_;
    using super_::addFlow_;
    using super_::netflow;
    using typename super_::flow_type;


    KtMaxFlowPre(const GRAPH& g, unsigned s, unsigned t) : super_(g, s, t) {

        // 各顶点的高度
        std::vector<unsigned> height(g.order(), 0);    
        height[s] = g.order();

        // 各顶点的超额量
        // 源点的超额量初始化为足够大，以便在第一次循环时尽最大流量压入合格边。
        std::vector<flow_type> excessFlow(g.order(), 0);
        excessFlow[s] = std::numeric_limits<flow_type>::max();

        // 活动顶点集，初始化为源点
        std::queue<unsigned> activeVert; 
        activeVert.push(s);


        // 预流-压入算法主循环
        while (!activeVert.empty()) {

            unsigned v = activeVert.front(); activeVert.pop();
            if (excessFlow[v] == 0)
                continue;

            auto iter = KtAdjIter(rg_, v);

            // 由于addFlow_会动态删增边，这将破坏迭代器数据，所以先统一收集邻接点
            std::vector<std::pair<unsigned, flow_type>> adjs;
            for (; !iter.isEnd(); ++iter)
                adjs.emplace_back(*iter, iter.edge());

            for(const auto& i : adjs) {
                unsigned w = i.first;
                auto pushFlow = std::min(i.second, excessFlow[v]);

                // 对合格边进行流压入操作
                if (pushFlow > 0 && (v == s || height[v] == height[w] + 1)) {
                    addFlow_(v, w, pushFlow);
                    excessFlow[v] -= pushFlow, excessFlow[w] += pushFlow;
                    if (w != s && w != t) {
                        assert(netflow(w) == excessFlow[w]);
                        activeVert.push(w);
                    }
                }
            }

            // 如果v仍有超额量（必定没有合格边），增加v的高度，并重新将v加到活动顶点集
            if (v != s && v != t && excessFlow[v] > 0) {
                assert(netflow(v) == excessFlow[v]);
                height[v]++;
                activeVert.push(v);
            }
        }
    }
};

