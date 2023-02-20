#pragma once
#include <functional>
#include "KuMath.h"


// ���ܶȹ��ƣ�Kernel Density Estimation��
// ʵ�ֲο���https://juejin.cn/post/6964279310823718943

template<typename KREAL>
class KtKde
{
public:

	using GETTER = std::function<KREAL(unsigned)>;
	using KERNEL = std::function<KREAL(KREAL)>;

	// @getter: ���ڻ�ȡ�������functor
	// @count: ��������
	// @kor: ���ڼ���˺�����functor.
	KtKde(GETTER getter, unsigned count, KERNEL kor);

	// ��������
	KtKde(const KtKde& rhs) {
		data_ = rhs.data_;
		kor_ = rhs.kor_;
		count_ = rhs.count_;
		h_ = rhs.h_;
	}

	KtKde(KtKde&& rhs) {
		std::swap(data_, rhs.data_);
		kor_ = rhs.kor_;
		count_ = rhs.count_;
		h_ = rhs.h_;
	}

	// ����pdf��x����ֵ
	KREAL operator()(const KREAL& x) const;

private:

	KREAL calcWinLength_();

private:
	std::vector<KREAL> data_; // ˳����������У��������pdf�ļ���Ч��
	KERNEL kor_;
	unsigned count_; 
	KREAL h_; // ����������Ĵ��ڳ���/����
};


// ����kernel���ؼ����ڼ���h�������ڣ�
template<typename KREAL>
KtKde<KREAL>::KtKde(GETTER getter, unsigned count, KERNEL kor)
{
	data_.resize(count);
	for (unsigned i = 0; i < count; i++)
		data_[i] = getter(i);
	std::sort(data_.begin(), data_.end());

	kor_ = kor;
	count_ = count;

	// �������
	h_ = calcWinLength_();
}


// pdf(x) = sum{ K(|x - x[i]| / h) } / (N * h), i = 1, ..., N
template<typename KREAL>
KREAL KtKde<KREAL>::operator()(const KREAL& x) const
{
	if (count_ * h_ == 0)
		return 0;

	KREAL sumk(0);

	auto start = std::lower_bound(data_.begin(), data_.end(), x - h_);
	if (start == data_.end())
		return 0;

	auto end = std::upper_bound(data_.begin(), data_.end(), x + h_);
	for (; start != end; start++)
		sumk += kor_(std::abs(x - *start) / h_);
	
	return sumk / (count_ * h_);
}


template<typename KREAL>
KREAL KtKde<KREAL>::calcWinLength_()
{
	// ����ArcGIS���������㷨
	// h = 0.9 * min(A, sqrt(1/ln(2)) * Dm)) * pow(n, -0.2)
	// ʽ�У�AΪ������׼���DmΪ����������ֵ�������λ��

	auto mean = KuMath::mean(data_.data(), count_);
	auto var = KuMath::var(data_.data(), count_, mean);
	auto A = std::sqrt(var);

	KREAL totalDist(0);
	for (unsigned i = 0; i < count_; i++)
		totalDist += std::abs(data_[i] - mean);
	auto Dm = totalDist / count_; // �˴��þ�ֵ�����λ��

	const static KREAL c = std::sqrt(1. / std::log(2.));
	return 0.9 * std::min(A, c * Dm) * std::pow(double(count_), -0.2);
}
