#pragma once
#include "KtDfsIter.h"


// KtDfsIter的加强版，总是stopAtPopping，主要新增2项新功能支持：
//   一是新增顶点的最小前序编号支持（low值），表示该顶点为根的子树中任何回边所引用的最小前序编号（order值）
//   二是新增判断当前顶点与其父顶点是否为桥的支持
// low值定义： low(v) = min(pushOrder[v], low[w], pushOrder[k])  
//   式中：w是顶点v的孩子节点，k是顶点v的回边祖先节点
template<typename GRAPH, bool fullGraph = false>
class KtDfsIterX : public KtDfsIter<GRAPH, false, true, true>
{
    using super_ = KtDfsIter<GRAPH, false, true, true>;

public:

    KtDfsIterX(GRAPH& graph, unsigned v = 0) : super_(graph, v), low_(graph.order(), -1) {
        low_[v] = pushingIndex(); 
    }

    using super_::pushingIndex;
    using super_::pushIndex;
    using super_::from;
    using super_::isEnd;
    using super_::isPushing;
    using super_::isPopping;
    using super_::isBack;
    using super_::firstUnvisited;


    // 重写父类的++操作符重载
    void operator++() { 
        super_::operator++();

        if(!isEnd()) {
            unsigned w = **this;
            
            if(isPushing()) {
                assert(pushIndex(w) == -1);
                low_[w] = pushingIndex(); // 初始化low(v) = pushOrder[v]
            }
            else {
                unsigned v = from();
                if (v != -1) {
                    if (isBack()) {
                        if (low_[v] > pushIndex(w))
                            low_[v] = pushIndex(w); // low(v) = min(low[v], pushOrder[k])   
                    }
                    else if (low_[v] > low_[w])
                        low_[v] = low_[w]; // low(v) = min(low[v], low[w]) 
                }
            }  
        }
        else if (fullGraph) {
            unsigned unvisted = firstUnvisited();
            if (unvisted != -1)
                start(unvisted);
        }
    }


    // 重写父类的start方法，以同步更新low值
    void start(unsigned v) {
        super_::start(v);
        low_[v] = pushingIndex();
    }


    unsigned lowIndex(unsigned v) const {
        return low_[v];
    }

    void resetLowIndex(unsigned v) {
        low_[v] = -1;
    }


    bool isBridge() const {
        unsigned v = **this;
        return /*from() != -1 && */isPopping() && lowIndex(v) == pushIndex(v);
    }


private:
    std::vector<unsigned> low_; // 存储各节点展开子孙节点所连接节点的最小序号
};