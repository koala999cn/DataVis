#pragma once
#include <array>


// std::array����ǿ�棬�ṩ�˶������͵Ĺ��캯��

template<typename T, int DIM>
class KtArray : public std::array<T, DIM>
{
	using super_ = std::array<T, DIM>;

public:

	using super_::super_;
	using super_::data;
	using super_::size;

	// �㹹�� std::arrayʵ��û��
	KtPoint() {
		super_::fill(0); // TODO: �Ƿ��Ҫ? 
	}

	// ����Ԫ�ض����valֵ
	KtPoint(const T& val) {
		super_::fill(val);
	}

	// ������ָ�빹��
	KtPoint(const T* buf) {
		std::copy(buf, buf + DIM, super_::begin());
	}

	// �ӵ��������죬��������ת��
	template<typename ITER>
	KtPoint(ITER iter) {
		std::copy(iter, iter + DIM, super_::begin());
	}

	// ��Ԫ��ֵ����
	template<typename... ARGS,
		std::enable_if_t<sizeof...(ARGS) == DIM, bool> = true> // �Ӹ�enable_if, �����ϸ������������²��ᱻ����
		KtPoint(ARGS... args) : super_{ static_cast<T>(args)... } {}

	constexpr static int dim() { return DIM; }

};