#pragma once
#include <array>


// std::array����ǿ�棬�ṩ�˶������͵Ĺ��캯��

template<typename T, int SIZE>
class KtArray : public std::array<T, SIZE>
{
	using super_ = std::array<T, SIZE>;

public:

	using super_::super_;
	using super_::data;
	using super_::size;

	// �㹹�� std::arrayʵ��û��
	KtArray() : super_{ 0 } {
		
	}

	// ����Ԫ�ض����valֵ
	template<typename U>
	explicit KtArray(const U& val) {
		super_::fill(T(val));
	}

	// �ӵ��������죬��������ת��
	template<typename ITER, typename = decltype(*ITER)>
	explicit KtArray(ITER iter) {
		std::copy(iter, iter + SIZE, super_::begin());
	}

	template<typename U>
	explicit KtArray(const KtArray<U, SIZE>& other) 
	    : KtArray(other.cbegin()) { }

	// ��Ԫ��ֵ����
	template<typename... ARGS,
		std::enable_if_t<sizeof...(ARGS) == SIZE, bool> = true> // �Ӹ�enable_if, �����ϸ������������²��ᱻ����
	KtArray(ARGS... args) : super_{ static_cast<T>(args)... } {}
};
