#pragma once
#include <map>


// 基于std::map实现的连续映射类
// 各离散点（关键点）保存在map中，各关键点之间使用线性插值

template<typename KEY, typename VALUE>
class KtGradient
{
public:

	using key_type = KEY;
	using value_type = VALUE;
	
	// 设置控制点
	void insert(const KEY& from, const VALUE& to) {
		stops_[from] = to;
	}

	// 移除控制点from
	void erase(const KEY& from) {
		stops_.erase(from);
	}

	// 是否存在控制点from
	bool has(const KEY& from) {
		return stops_.find(from) != stops_.cend();
	}

	// 移动控制点
	void move(const KEY& oldFrom, const KEY& newFrom) {
		assert(has(oldFrom));
		auto val = stops_[oldFrom];
		erase(oldFrom);
		insert(newFrom, val);
	}

	// 连续映射
	VALUE map(const KEY& from) const {
		assert(!stops_.empty());

		auto upper = stops_.upper_bound(from);
		if (upper == stops_.cend())
			return std::prev(stops_.cend(), 1)->second;
		else if (upper == stops_.cbegin())
			return upper->second;

		auto lower = std::prev(upper);

		auto& x0 = lower->first;
		auto& x1 = upper->first;
		auto& y0 = lower->second;
		auto& y1 = upper->second;

		assert(x0 != x1);
		return y0 + (y1 - y0) * ((from - x0) / (x1 - x0));
	}

	unsigned size() const {
		return stops_.size();
	}

	auto& at(unsigned idx) const {
		return *std::next(stops_.cbegin(), idx);
	}

	void clear() {
		stops_.clear();
	}

	auto begin() const { return stops_.begin(); }
	auto end() const { return stops_.end(); }

	auto cbegin() const { return stops_.cbegin(); }
	auto cend() const { return stops_.cend(); }

private:
	std::map<KEY, VALUE> stops_;
};

