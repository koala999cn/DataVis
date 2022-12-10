#pragma once
#include "KtArray.h"


// �ṩ����һ���Ե��������ҷ�λ����

template<typename T>
class KtMargins : public KtArray<T, 4>
{
private:
	using super_ = KtArray<T, 4>;

public:

	using super_::super_;

	KtMargins(T l, T t, T r, T b) {
		left() = l, right() = r, top() = t, bottom() = b;
	}
	
	const T& left() const { return super_::at(0); }
	T& left() { return super_::at(0); }

	const T& top() const { return super_::at(1); }
	T& top() { return super_::at(1); }

	const T& right() const { return super_::at(2); }
	T& right() { return super_::at(2); }

	const T& bottom() const { return super_::at(3); }
	T& bottom() { return super_::at(3); }

	bool isNull() const {
		return left() == 0 && top() == 0 && 
			right() == 0 && bottom() == 0;
	}

	void swapLeftRight() {
		std::swap(left(), right());
	}

	void swapBottomTop() {
		std::swap(top(), bottom());
	}
};
