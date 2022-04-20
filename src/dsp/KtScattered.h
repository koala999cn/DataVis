#pragma once
#include "KvData.h"
#include <array>
#include <vector>
#include "KtuMath.h"


template<int DIM>
class KtScattered : public KvData
{
public:
	using index = std::array<kReal, DIM>;

	constexpr kIndex dim() const override {
		return DIM;
	}

	kIndex count() const override {
		return static_cast<kIndex>(inds_.size());
	}

	kIndex channels() const override {
		return static_cast<kIndex>(vals_.size());
	}

	void clear() override {
		inds_.clear();
		for (auto& v : vals_)
			v.clear();
	}

	kIndex size(kIndex axis) const override {
		return count();
	}

	kRange range(kIndex axis) const override {
		if (axis < DIM)
		    return empty() ? { 0, 0 } : { inds_[0][axis], inds_.back()[axis] };
		else 
			return valueRange();
	}

	kReal step(kIndex axis) const override {
		return k_nonuniform_step;
	}

	kReal value(kIndex idx[], kIndex channel) const override {
		return vals_[channel][idx[0]];
	}

	std::vector<kReal> point(kIndex idx[], kIndex channel) const override {
		std::vector<kReal> pt(inds_[idx[0]]);
		pt.push_back(value(idx, channel));
		return pt;
	}

	kReal value(kReal pt[], kIndex channel) const override {
		assert(false);
		return 0; // TODO
	}


	kRange valueRange(kIndex channel) const {
		auto r = KtuMath<kReal>::minmax(vals_.data(), count());
		return { r.first, r.second };
	}

	kRange valueRange() const {
		auto r = valueRange(0);
		for (kIndex c = 1; c < channels(); c++) {
			auto rc = valueRange(c);
			if (rc.low() < r.low())
				r.resetLow(rc.low());
			if (rc.high() > r.high())
				r.resetHigh(rc.high());
		}

		return r;
	}

private:
	std::vector<index> inds_;
	std::vector<std::vector<kReal>> vals_;
};

