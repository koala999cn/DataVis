#pragma once
#include <array>
#include "KtuMath.h"


// std::array����ǿ�棬�ṩ�˶������͵Ĺ��캯�����Լ�Ԫ�����еıȽϺ���

template<typename T, int SIZE>
class KtArray : public std::array<T, SIZE>
{
	using super_ = std::array<T, SIZE>;
	using kMath = KtuMath<T>;

public:

	using super_::super_;
	using super_::data;

	// �㹹�� std::arrayʵ��û��
	KtArray() : super_{ 0 } {
		
	}

	// ����Ԫ�ض����valֵ
	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	explicit KtArray(const U& val) {
		super_::fill(T(val));
	}

	// �ӵ��������죬��������ת��
	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
	explicit KtArray(const U* data) {
		for (unsigned i = 0; i < SIZE; i++)
			at(i) = static_cast<T>(*data++);
	}

	template<typename U>
	KtArray(const KtArray<U, SIZE>& other) 
	    : KtArray(other.data()) { }

	// ��Ԫ��ֵ����
	template<typename... ARGS,
		std::enable_if_t<sizeof...(ARGS) == SIZE, bool> = true> // �Ӹ�enable_if, �����ϸ������������²��ᱻ����
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

	// ȡthis��pt��ά�ȵĸ�ֵ
	void makeCeil(const KtArray& pt) {
		*this = ceil(*this, pt);
	}

	// ȡthis��pt��ά�ȵĵ�ֵ
	void makeFloor(const KtArray& pt) {
		*this = floor(*this, pt);
	}

	static const KtArray& zero() {
		static KtArray o;
		return o;
	}

	// ȡpt1��pt2��ά�ȵĸ�ֵ
	static KtArray ceil(const KtArray& pt1, const KtArray& pt2) {
		KtArray pt;
		kMath::forEach(pt1.data(), pt2.data(), pt.data(), size(), [](T x, T y) {
			return std::max(x, y);
			});
		return pt;
	}

	// ȡpt1��pt2��ά�ȵĵ�ֵ
	static KtArray floor(const KtArray& pt1, const KtArray& pt2) {
		KtArray pt;
		kMath::forEach(pt1.data(), pt2.data(), pt.data(), size(), [](T x, T y) {
			return std::min(x, y);
			});
		return pt;
	}
};
