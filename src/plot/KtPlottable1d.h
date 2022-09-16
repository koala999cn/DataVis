#pragma once
#include <vector>
#include "KvPlottable.h"
#include "KtColor.h"


//
// 一维图的抽象类
// 此处的一维主要指数据按照串行方式组织，每个数据点可以是不同维度
// @DIM: 数据点的维度，一般为2或者3
//

template<int DIM>
class KtPlottable1d : public KvPlottable
{
	using super_ = KvPlottable;

public:
	using point_type = KtPoint<double, DIM>;
	using data_type = std::vector<point_type>;

	using super_::super_;

	const data_type& data() const { return data_; }
	void setData(const data_type& data) { data_ = data; }
	void setData(data_type&& data) { data_ = std::move(data); }

private:
	color4f major_, minor_;
	data_type data_;
};
