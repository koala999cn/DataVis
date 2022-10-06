#pragma once
#include <array>


// std::array的增强版，提供了多种类型的构造函数

template<typename T, int SIZE>
class KtArray : public std::array<T, SIZE>
{
	using super_ = std::array<T, SIZE>;

public:

	using super_::super_;
	using super_::data;
	using super_::size;

	// 零构造 std::array实现没？
	KtArray() : super_{ 0 } {
		
	}

	// 所有元素都填充val值
	template<typename U>
	explicit KtArray(const U& val) {
		super_::fill(T(val));
	}

	// 从迭代器构造，方便类型转换
	template<typename ITER, typename = decltype(*ITER)>
	explicit KtArray(ITER iter) {
		std::copy(iter, iter + SIZE, super_::begin());
	}

	template<typename U>
	explicit KtArray(const KtArray<U, SIZE>& other) 
	    : KtArray(other.cbegin()) { }

	// 从元素值构造
	template<typename... ARGS,
		std::enable_if_t<sizeof...(ARGS) == SIZE, bool> = true> // 加个enable_if, 否则上个构造多数情况下不会被调用
	KtArray(ARGS... args) : super_{ static_cast<T>(args)... } {}
};
