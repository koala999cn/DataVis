#pragma once
#include "KvData.h"


// 离散数据抽象类，与KvContinued相对应
class KvDiscreted : public KvData
{
public:

	// 返回axis维度的数据数量
	virtual kIndex size(kIndex axis) const = 0;

	// 返回axis维度的步进，即dx, dy, dz...
	// 返回0表示步进非均匀，对应于散点数据
	virtual kReal step(kIndex axis) const = 0;
	constexpr static kReal k_nonuniform_step = 0;


	// 通过索引获取数据值
	// @idx: 大小为dim，各元素分表表示对应坐标轴的数据点索引
	virtual kReal value(kIndex idx[], kIndex channel) const = 0;

	// 参数同上，不同的是返回数据数组，含有各坐标轴的数据值
	virtual std::vector<kReal> point(kIndex idx[], kIndex channel) const = 0;

	// 清空数据
	virtual void clear() = 0;

	// 数据为空？
	virtual bool empty() const { return count() == 0; }


	// 是否散点数据？
	bool isScattered() const {
		return step(0) == k_nonuniform_step;
	}

	// 是否采样数据
	bool isSampled() const {
		return step(0) != k_nonuniform_step;
	}


	/// 几个helper函数

	kReal value(kIndex idx, kIndex channel) const {
		assert(dim() == 1);
		return value(&idx, channel);
	}

	kReal value(kIndex idx0, kIndex idx1, kIndex channel) const {
		assert(dim() == 2);
		kIndex idx[2] = { idx0, idx1 };
		return value(idx, channel);
	}

	std::vector<kReal> point(kIndex idx, kIndex channel) const {
		assert(dim() == 1);
		return point(&idx, channel);
	}

	std::vector<kReal> point(kIndex idx0, kIndex idx1, kIndex channel) const {
		assert(dim() == 2);
		kIndex idx[2] = { idx0, idx1 };
		return point(idx, channel);
	}
};