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

	kIndex dim() const override {
		return DIM;
	}

	kIndex size() const override {
		return static_cast<kIndex>(data_.size() / channles_);
	}

	kIndex channels() const override {
		return channles_; 
	}

	void resize(kIndex shape[], kIndex chs = 0) override {
		if (chs == 0)
			chs = channels();
		kIndex sz = shape == nullptr ? size() : shape[0];
	    data_.resize(sz * chs); // TODO: 转移数据
		channles_ = chs;
	}

	void clear() override {
		data_.clear();
	}

	kIndex size(kIndex axis) const override {
		return size(); // TODO:
	}

	kRange range(kIndex axis) const override {
		if (empty()) return { 0, 0 };

		kReal low = std::numeric_limits<kReal>::max();
		kReal high = std::numeric_limits<kReal>::lowest();

		for (kIndex i = 0; i < data_.size(); i++) {
			auto val = data_[i][axis];
			if (std::isnan<kReal>(val))
				continue;

			low = std::min(low, val);
			high = std::max(high, val);
		}

		return { low, high };
	}

	kReal step(kIndex axis) const override {
		return k_nonuniform_step;
	}

	kReal valueAt(kIndex n, kIndex channel) const override {
		return data_[n * channels() + channel].back();
	}

	std::vector<kReal> pointAt(kIndex n, kIndex channel) const override {
		std::vector<kReal> pt(dim() + 1);
		auto offset = n * channels() + channel;
		std::copy(data_[offset].cbegin(), data_[offset].cend(), pt.begin());
		return pt;
	}

	//kReal value(kIndex idx[], kIndex channel) const override {
		////return vals_[channel][idx[0]];
	//	assert(false);
	//	return 0;
	//}

	//std::vector<kReal> point(kIndex idx[], kIndex channel) const override {
		//std::vector<kReal> pt(inds_[idx[0]]);
		//pt.push_back(value(idx, channel));
		//return pt;
	//	assert(false);
	//	return { 0 };
	//}

	// TODO: test
	kReal xToIndex(kReal x) const override {
		// TODO: 目前只处理第0通道
		auto cmp = [](const element_type& e1, const element_type& e2) {
			for (auto i = 0; i < e1.size(); i++) {
				if (e1[i] < e2[i])
					return true;
				else if (e1[i] > e2[i])
					return false;
			}

			// 相等
			return true;
		};

		assert(std::is_sorted(data_.cbegin(), std::next(data_.cbegin(), size())));

		if (empty()) return 0;

		auto N = size();
		auto cend = std::next(data_.cbegin(), N);
		auto pos = std::lower_bound(data_.cbegin(), cend, x,
			[](const element_type& e, kReal x) { return e.front() < x; });

		auto diff = std::distance(pos, data_.cbegin());
		if (diff > N - 1) 
			diff = N - 1;	
		auto off = x - data_[diff].front();
		auto dx = diff == 0 ? data_[diff + 1].front() - data_[diff].front() 
			                : data_[diff].front() - data_[diff - 1].front();
		assert(dx != 0); // TODO: 处理相等情况
		return diff + off / dx;
	}


	/// 写操作

	void reserve(kIndex frames) {
		data_.reserve(frames * channels());
	}

	void pushBack(const element_type e[]) {
		for (kIndex c = 0; c < channels(); c++)
		    data_.push_back(e[c]);
	}

	// 单通道的快捷方法
	void pushBack(const element_type& e) {
		assert(channels() == 1);
		data_.push_back(e);
	}

private:
	std::vector<element_type> data_;
	kIndex channles_;
};

