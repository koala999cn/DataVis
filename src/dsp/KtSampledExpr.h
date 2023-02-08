#pragma once
#include <array>
#include <memory>
#include "KtSampled.h"
#include "KvContinued.h"
#include "KuMath.h"


// 与KtSampledArray最大不同之处在于，KtSampledExpr从采样维度映射到另外的函数表达式
// 因此，采样维度不作为point值返回，而是有另外的映射值
// 比如，当DIM = 2时，返回值x, y, z计算如下:
// x = f(u, v), y = g(u, v), z = h(u, v)
// 式中的u， v分别为0维度和1维度的采样点值
//
// 为简单起见，目前限制输出维度 = 输入维度

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
			out[i] = exprs_[i]->value(in.data(), channel); // TODO: 目前始终0通道
		return out;
	}

	kRange range(kIndex axis) const override {
		assert(axis >= 0 && axis <= dim());
		return exprs_[axis]->valueRange();
	}

	// 同步exprs_
	void resize(kIndex shape[], kIndex chs = 0) override {
		assert(chs == 0 || chs == channels()); // 无法重置channels
		super_::resize(shape, chs);
		for (unsigned i = 0; i < DIM + 1; i++)
			for (unsigned j = 0; j < DIM; j++)
			    exprs_[i]->setRange(j, samp_[j].low(), samp_[j].high());
	}

	// 同步exprs_
	void reset(kIndex axis, kReal low, kReal step, kReal x0_ref = 0) override {
		super_::reset(axis, low, step, x0_ref);
		for (unsigned i = 0; i < DIM + 1; i++)
			exprs_[i]->setRange(axis, samp_[axis].low(), samp_[axis].high());
	}

	// 低效率实现，尽量不要调用，而且value的语义也不清楚，目前使用最高维度的数据值
	kReal value(kIndex idx[], kIndex channel) const override {
		return point(idx, channel).back();
	}

	void setExpr(unsigned axis, expr_t expr) {
		exprs_[axis] = expr;
	}

private:
	expr_array_t exprs_; // 将采样点映射到数据点的函数表达式，即f, g, h, ...
};

