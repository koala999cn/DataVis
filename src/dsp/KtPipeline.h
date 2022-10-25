#pragma once
#include <assert.h>
#include "KtHolder.h"


// 流水线操作
// 每个OP算子具有3个成员方法：idim, odim, process
// KtPipeline本身也可作为OP加入到更上一级的pipeline

template<typename... OP>
class KtPipeline : public KtTupleHolder<OP...>
{
public:
	using super_ = KtTupleHolder<OP...>;
	using super_::super_;
	using super_::tupleSize;
	using super_::inside;

	unsigned idim() const {
		return inside<0>().idim();
	}

	unsigned odim() const {
		return inside<tupleSize()-1>().odim();
	}

	void process(const double* in, double* out) const {
		doPipeline<tupleSize()>(in, out);
	}


	template<int N>
	void doPipeline(const double* buf, double* out) const {
		auto& op = inside<N - 1>();
		assert(op.idim() == inside<N - 2>().odim());
		std::vector<double> temp(op.idim());
		doPipeline<N - 1>(buf, temp.data());
		op.process(temp.data(), out);
	}

	template<>
	void doPipeline<1>(const double* buf, double* out) const {
		inside<0>().process(buf, out);
	}
};
