#pragma once
#include <array>
#include <memory>
#include "KtSampled.h"
#include "KvContinued.h"
#include "KuMath.h"


// ��KtSampledArray���֮ͬ�����ڣ�KtSampledExpr�Ӳ���ά��ӳ�䵽����ĺ������ʽ
// ��ˣ�����ά�Ȳ���Ϊpointֵ���أ������������ӳ��ֵ
// ���磬��DIM = 2ʱ������ֵx, y, z��������:
// x = f(u, v), y = g(u, v), z = h(u, v)
// ʽ�е�u�� v�ֱ�Ϊ0ά�Ⱥ�1ά�ȵĲ�����ֵ
//
// Ϊ�������Ŀǰ�������ά�� = ����ά��

template<int DIM>
class KtSampledExpr : public KtSampled<DIM>
{
public:
    using super_ = KtSampled<DIM>;
    using expr_t = std::shared_ptr<KvContinued>;
	using expr_array_t = std::array<expr_t, DIM + 1>;

    KtSampledExpr(const expr_array_t& exprs) : exprs_(exprs) {}

	kIndex channels() const override {
		return exprs_.front()->channels();
	}

	std::vector<kReal> point(kIndex idx[], kIndex channel) const override {
		std::vector<kReal> in(DIM);
		for (kIndex i = 0; i < DIM; i++)
			in[i] = indexToValue(i, idx[i]);

		std::vector<kReal> out(DIM + 1);
		for (unsigned i = 0; i < DIM + 1; i++)
			out[i] = exprs_[i]->value(in.data(), channel); // TODO: Ŀǰʼ��0ͨ��
		return out;
	}

	kRange range(kIndex axis) const override {
		assert(axis >= 0 && axis <= dim());
		return exprs_[axis]->valueRange();
	}

	// ͬ��exprs_
	void resize(kIndex shape[], kIndex chs = 0) override {
		assert(chs == 0 || chs == channels()); // �޷�����channels
		super_::resize(shape, chs);
		for (unsigned i = 0; i < DIM + 1; i++)
			for (unsigned j = 0; j < DIM; j++)
			    exprs_[i]->setRange(j, samp_[j].low(), samp_[j].high());
	}

	// ͬ��exprs_
	void reset(kIndex axis, kReal low, kReal step, kReal x0_ref = 0) override {
		super_::reset(axis, low, step, x0_ref);
		for (unsigned i = 0; i < DIM + 1; i++)
			exprs_[i]->setRange(axis, samp_[axis].low(), samp_[axis].high());
	}

	// ��Ч��ʵ�֣�������Ҫ���ã�����value������Ҳ�������Ŀǰʹ�����ά�ȵ�����ֵ
	kReal value(kIndex idx[], kIndex channel) const override {
		return point(idx, channel).back();
	}

	void setExpr(unsigned axis, expr_t expr) {
		exprs_[axis] = expr;
	}

private:
	expr_array_t exprs_; // ��������ӳ�䵽���ݵ�ĺ������ʽ����f, g, h, ...
};

