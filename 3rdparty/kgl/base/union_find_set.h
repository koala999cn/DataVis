#pragma once
#include <vector>


// ���鼯ʵ��
class union_find_set
{
public:
    union_find_set(unsigned size) : id_(size), ranks_(size, 0) {
        for(unsigned i = 0; i < size; i++)
            id_[i] = i;
    }


    // ����Ԫ��i�������ϵ�id
    unsigned find(unsigned i)  {
        while(i != id_[i]) 
            i = id_[i] = id_[id_[i]]; // ˳�����·��ѹ��
        return i;
    }


    // ��Ԫ��i����������Ԫ��j�������Ϻϲ�
    // ����false��ʾi��j��������ͬһ���ϣ�δִ�кϲ�����
    bool unite(unsigned i, unsigned j) {
        unsigned si = find(i), sj = find(j);
        if(si == sj) return false;

        // ranksֵ��ĺϲ�ranksֵС��
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