#pragma once
#include <array>
#include "KtuMath.h"


// std::array的增强版，提供了多种类型的构造函数，以及元素序列的比较函数

template<typename T, int SIZE>
class KtArray : public std::array<T, SIZE>
{
	using super_ = std::array<T, SIZE>;
	using kMath = KtuMath<T>;

public:

	using super_::super_;
	using super_::data;

	// 零构造 std::array实现没？
	KtArray() : super_{ 0 } {
		
	}

	// 所有元素都填充val值
	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	explicit KtArray(const U& val) {
		super_::fill(T(val));
	}

	// 从迭代器构造，方便类型转换
	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	explicit KtArray(const U* data) {
		for (unsigned i = 0; i < SIZE; i++)
			at(i) = static_cast<T>(*data++);
	}

	template<typename U>
	KtArray(const KtArray<U, SIZE>& other) 
	    : KtArray(other.data()) { }

	// 从元素值构造
	template<typename... ARGS,
		std::enable_if_t<sizeof...(ARGS) == SIZE, bool> = true> // 加个enable_if, 否则上个构造多数情况下不会被调用
	KtArray(ARGS... args) : super_{ static_cast<T>(args)... } {}

	constexpr static unsigned size() { return SIZE; }

	operator const T* () const { return data(); }
	operator T* () { return data(); }

	// return ture when this[i] < rhs[i] for all i
	bool lt(const KtArray& rhs) const {
		for (unsigned i = 0; i < size(); i++)
			if (at(i) >= rhs.at(i))
				return false;
		return true;
	}

	// return ture when this[i] > rhs[i] for all i
	bool gt(const KtArray& rhs) const {
		for (unsigned i = 0; i < size(); i++)
			if (at(i) <= rhs.at(i))
				return false;
		return true;
	}

	// return ture when this[i] <= rhs[i] for all i
	bool le(const KtArray& rhs) const {
		for (unsigned i = 0; i < size(); i++)
			if (at(i) > rhs.at(i))
				return false;
		return true;
	}

	// return ture when this[i] >= rhs[i] for all i
	bool ge(const KtArray& rhs) const {
		for (unsigned i = 0; i < size(); i++)
			if (at(i) < rhs.at(i))
				return false;
		return true;
	}

	bool bewteen(const KtArray& low, const KtArray& high) const {
		return ge(low) && le(high);
	}

	// 取this和pt各维度的高值
	void makeCeil(const KtArray& pt) {
		*this = ceil(*this, pt);
	}

	// 取this和pt各维度的低值
	void makeFloor(const KtArray& pt) {
		*this = floor(*this, pt);
	}

	static const KtArray& zero() {
		static KtArray o;
		return o;
	}

	// 取pt1和pt2各维度的高值
	static KtArray ceil(const KtArray& pt1, const KtArray& pt2) {
		KtArray pt;
		kMath::forEach(pt1.data(), pt2.data(), pt.data(), size(), [](T x, T y) {
			return std::max(x, y);
			});
		return pt;
	}

	// 取pt1和pt2各维度的低值
	static KtArray floor(const KtArray& pt1, const KtArray& pt2) {
		KtArray pt;
		kMath::forEach(pt1.data(), pt2.data(), pt.data(), size(), [](T x, T y) {
			return std::min(x, y);
			});
		return pt;
	}
};
