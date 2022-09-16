#pragma once
#include <vector>
#include "KvPlottable.h"
#include "KtColor.h"


//
// һάͼ�ĳ�����
// �˴���һά��Ҫָ���ݰ��մ��з�ʽ��֯��ÿ�����ݵ�����ǲ�ͬά��
// @DIM: ���ݵ��ά�ȣ�һ��Ϊ2����3
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
