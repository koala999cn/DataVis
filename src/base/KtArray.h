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
	KtPoint() {
		super_::fill(0); // TODO: 是否必要? 
	}

	// 所有元素都填充val值
	explicit KtPoint(const T& val) {
		super_::fill(val);
	}

	// 从数据指针构造
	explicit KtPoint(const T* buf) {
		std::copy(buf, buf + DIM, super_::begin());
	}

	// 从迭代器构造，方便类型转换
	template<typename ITER>
	explicit KtPoint(ITER iter) {
		std::copy(iter, iter + DIM, super_::begin());
	}

	// 从元素值构造
	template<typename... ARGS,
		std::enable_if_t<sizeof...(ARGS) == DIM, bool> = true> // 加个enable_if, 否则上个构造多数情况下不会被调用
		KtPoint(ARGS... args) : super_{ static_cast<T>(args)... } {}
};