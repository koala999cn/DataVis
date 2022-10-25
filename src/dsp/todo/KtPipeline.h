#pragma once
#include <assert.h>
#include "KtHolder.h"


// ��ˮ�߲���
// ÿ��OP���Ӿ���3����Ա������idim, odim, process
// KtPipeline����Ҳ����ΪOP���뵽����һ����pipeline

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
