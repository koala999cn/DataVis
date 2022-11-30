#pragma once
#include <type_traits>
#include <unordered_map>
#include <assert.h>


// X <-> Y的双向映射，双向都必须是一对一的
template<typename X, typename Y>
class bimap
{
	static_assert(!std::is_same<X, Y>::value, "X and Y cannot be the same type.");

public:
	bimap() = default;

	bimap(size_t mapSize) { reserve(mapSize); }

	template<int N>
	bimap(const std::pair<X, Y>(&elements)[N]) {
		reserve(N);
		for (int i = 0; i < N; i++)
			insert(elements[i].first, elements[i].second);
	}

	void reserve(size_t mapSize) {
		x2y_.reserve(size_t(mapSize * 1.25));
		y2x_.reserve(size_t(mapSize * 1.25));
	}

	void clear() {
		x2y_.clear(), y2x_.clear();
	}

	auto size() const { return x2y_.size(); }

	void insert(const X& x, const Y& y) {
		assert(!hasX(x) && !hasY(y));
		x2y_.insert_or_assign(x, y), y2x_.insert_or_assign(y, x);
	}

	void eraseByX(const X& x) {
		auto& y = x2y(x);
		y2x_.erase(y);
		x2y_.erase(x);	
	}

	void eraseByY(const Y& y) {
		auto& x = y2x(y);
		x2y_.erase(x);
		y2x_.erase(y);
	}


	const Y& x2y(const X& x) const {
		assert(hasX(x));
		return x2y_.find(x)->second;
	}

	const X& y2x(const Y& y) const {
		assert(hasY(y));
		return y2x_.find(y)->second;
	}


	bool hasX(const X& x) const {
		return x2y_.find(x) != x2y_.end();
	}

	bool hasY(const Y& y) const {
		return y2x_.find(y) != y2x_.end();
	}


	auto xyBegin() const { return x2y_.begin(); }
	auto yxBegin() const { return y2x_.begin(); }

	auto xyEnd() const { return x2y_.end(); }
	auto yxEnd() const { return y2x_.end(); }

private:
	std::unordered_map<X, Y> x2y_;
	std::unordered_map<Y, X> y2x_;
};

