#pragma once
#include <limits>

#undef max

// ��Ȩֵ�йصĲ����������ӱ�ֵ��Ȩֵ��ת����Ȩֵ�ıȽϣ�Ȩֵ���ۼ�

template<typename VALUE_TYPE>
class KtWeightSelf
{
public:
    using weight_type = VALUE_TYPE;
    weight_type operator()(const VALUE_TYPE& val) const {
        return val;
    }
};


template<typename VALUE_TYPE>
class KtWeightUnit
{
public:
    using weight_type = int;
    constexpr weight_type operator()(const VALUE_TYPE&) const {
        return 1;
    }
};


template<typename WEIGHT_TYPE>
class KtAdder
{
public:
    auto operator()(const WEIGHT_TYPE& w1, const WEIGHT_TYPE& w2) const {
        return w1 + w2;
    }
};


// @minimum��Ϊtrueʱ��Сֵ���ţ������ֵ���š�
template<class WeightorBase, class Accumlate, bool minimum>
class KtWeightor : public WeightorBase
{
public:
    using weight_type = typename WeightorBase::weight_type;
    const weight_type worst_weight = minimum ? 
        std::numeric_limits<weight_type>::max() : 
        std::numeric_limits<weight_type>::lowest();


    // w1�Ƿ��w2���ţ�
    bool comp(const weight_type& w1, const weight_type& w2) const {
        return minimum ? w1 < w2 : w1 > w2;
    }

    // Ȩֵ�ۼ�
    auto acc(const weight_type& w1, const weight_type& w2) const {
        return Accumlate{}(w1, w2);
    }
};


template<class WeightorBase, class Accumlate>
using KtWeightorMin = KtWeightor<WeightorBase, Accumlate, true>;


template<class WeightorBase, class Accumlate>
using KtWeightorMax = KtWeightor<WeightorBase, Accumlate, false>;


template<typename GRAPH>
using default_min_wtor = KtWeightorMin<KtWeightSelf<typename GRAPH::edge_type>, KtAdder<typename KtWeightSelf<typename GRAPH::edge_type>::weight_type>>;

template<typename GRAPH>
using default_max_wtor = KtWeightorMax<KtWeightSelf<typename GRAPH::edge_type>, KtAdder<typename KtWeightSelf<typename GRAPH::edge_type>::weight_type>>;

template<typename GRAPH>
using unit_min_wtor = KtWeightorMin<KtWeightUnit<typename GRAPH::edge_type>, KtAdder<typename KtWeightUnit<typename GRAPH::edge_type>::weight_type>>;

template<typename GRAPH>
using unit_max_wtor = KtWeightorMax<KtWeightUnit<typename GRAPH::edge_type>, KtAdder<typename KtWeightUnit<typename GRAPH::edge_type>::weight_type>>;


template<typename GRAPH>
using default_wtor = default_min_wtor<GRAPH>;

