#pragma once
#include <vector>


// 并查集实现
class union_find_set
{
public:
    union_find_set(unsigned size) : id_(size), ranks_(size, 0) {
        for(unsigned i = 0; i < size; i++)
            id_[i] = i;
    }


    // 查找元素i所属集合的id
    unsigned find(unsigned i)  {
        while(i != id_[i]) 
            i = id_[i] = id_[id_[i]]; // 顺便进行路径压缩
        return i;
    }


    // 将元素i所属集合与元素j所属集合合并
    // 返回false表示i和j本来就属同一集合，未执行合并操作
    bool unite(unsigned i, unsigned j) {
        unsigned si = find(i), sj = find(j);
        if(si == sj) return false;

        // ranks值大的合并ranks值小的
        if(ranks_[si] > ranks_[sj]) 
            id_[sj] = si;
        else if(ranks_[si] < ranks_[sj]) 
            id_[si] = sj;
        else {
            id_[sj] = si;
            ranks_[si]++;
        }
        
        return true;
    }


private:
    std::vector<unsigned> id_;
    std::vector<unsigned> ranks_;
};