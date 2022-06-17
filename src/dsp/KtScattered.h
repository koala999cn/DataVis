#pragma once
#include "KvDiscreted.h"
#include <array>
#include <vector>
#include "KtuMath.h"


template<int DIM>
class KtScattered : public KvDiscreted
{
public:
	using element_type = std::array<kReal, DIM + 1>;

	KtScattered() {
		channles_ = 1;
	}

	constexpr kIndex dim() const override {
		return DIM;
	}

	kIndex size() const override {
		return static_cast<kIndex>(data_.size() / channles_);
	}

	kIndex channels() const override {
		return channles_; 
	}

	void clear() override {
		data_.clear();
	}

	kIndex size(kIndex axis) const override {
		return size(); // TODO:
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

	kReal xToIndex(kReal x) const override {
		auto pos = std::lower_bound(inds_.begin(), inds_.end(), x, 
			[](kReal x, const index& idx) {
				return x < idx[0];
			});
	}

	kRange valueRange(kIndex channel) const override {
		auto r = KtuMath<kReal>::minmax(vals_.data(), size());
		return { r.first, r.second };
	}

private:
	std::vector<element_type> data_;
	kIndex channles_;
};

