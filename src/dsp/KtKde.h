#pragma once
#include <functional>
#include "KuMath.h"


// 核密度估计（Kernel Density Estimation）
// 实现参考：https://juejin.cn/post/6964279310823718943

template<typename KREAL>
class KtKde
{
public:

	using GETTER = std::function<KREAL(unsigned)>;
	using KERNEL = std::function<KREAL(KREAL)>;

	// @getter: 用于获取样本点的functor
	// @count: 样本总数
	// @kor: 用于计算核函数的functor.
	KtKde(GETTER getter, unsigned count, KERNEL kor);

	// 拷贝构造
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

	// 计算pdf在x处的值
	KREAL operator()(const KREAL& x) const;

private:

	KREAL calcWinLength_();

private:
	std::vector<KREAL> data_; // 顺序的样本序列，用以提高pdf的计算效率
	KERNEL kor_;
	unsigned count_; 
	KREAL h_; // 由样本估算的窗口长度/带宽
};


// 给定kernel，关键在于计算h（即窗口）
template<typename KREAL>
KtKde<KREAL>::KtKde(GETTER getter, unsigned count, KERNEL kor)
{
	data_.resize(count);
	for (unsigned i = 0; i < count; i++)
		data_[i] = getter(i);
	std::sort(data_.begin(), data_.end());

	kor_ = kor;
	count_ = count;

	// 计算带宽
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
	// 采用ArcGIS计算带宽的算法
	// h = 0.9 * min(A, sqrt(1/ln(2)) * Dm)) * pow(n, -0.2)
	// 式中：A为样本标准方差，Dm为各样本到均值距离的中位数

	auto mean = KuMath::mean(data_.data(), count_);
	auto var = KuMath::var(data_.data(), count_, mean);
	auto A = std::sqrt(var);

	KREAL totalDist(0);
	for (unsigned i = 0; i < count_; i++)
		totalDist += std::abs(data_[i] - mean);
	auto Dm = totalDist / count_; // 此处用均值替代中位数

	const static KREAL c = std::sqrt(1. / std::log(2.));
	return 0.9 * std::min(A, c * Dm) * std::pow(double(count_), -0.2);
}
