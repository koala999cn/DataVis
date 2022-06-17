#pragma once
#include "KvData.h"


// 离散数据抽象类，与KvContinued相对应
class KvDiscreted : public KvData
{
public:

	using KvData::valueRange;

	kRange valueRange(kIndex channel) const override;

	// @shape: 新设定的各维度尺寸, =nullptr表示不作调整
	// @channels: 新设定的通道数，=0表示不作调整
	virtual void resize(kIndex shape[], kIndex channels = 0) = 0;

	using KvData::size;

	// 返回axis维度的数据数量
	virtual kIndex size(kIndex axis) const = 0;

	// 返回axis维度的步进，即dx, dy, dz...
	// 返回0表示步进非均匀，对应于散点数据
	virtual kReal step(kIndex axis) const = 0;
	constexpr static kReal k_nonuniform_step = 0;

	// 清空数据
	virtual void clear() = 0;

	// 获取第n个数据的值
	// 0 <= n < size
	virtual kReal valueAt(kIndex n, kIndex channel) const = 0;

	// 获取第n个数据坐标点
	// 0 <= n < size
	virtual std::vector<kReal> pointAt(kIndex n, kIndex channel) const = 0;

	// 插值时须调用该函数
	virtual kReal xToIndex(kReal x) const = 0;

	/// 几个helper函数

	// 数据为空？
	bool empty() const { return size() == 0; }

	kIndex xToLowIndex(kReal x) const {
		return static_cast<kIndex>(std::floor(xToIndex(x)));
	}

	kIndex xToHighIndex(kReal x) const {
		return static_cast<kIndex>(std::ceil(xToIndex(x)));
	}

	kIndex xToNearestIndex(kReal x) const {
		return static_cast<kIndex>(std::round(xToIndex(x)));
	}

	// 是否散点数据？
	bool isScattered() const {
		return step(0) == k_nonuniform_step;
	}

	// 是否采样数据
	bool isSampled() const {
		return step(0) != k_nonuniform_step;
	}

};