#pragma once
#include <vector> // for std::pair
#include "kDsp.h" // for kReal
#include "KtInterval.h"
#include <assert.h>

using kRange = KtInterval<kReal>;


// 所有数据源的基类. 仅提供一个接口，即获取数据维度的方法
class KvData
{
public:

	virtual ~KvData() {}

	// 数据的维度
	virtual kIndex dim() const = 0; 

	// 包含的数据总数，返回inf对应连续数据
	virtual kIndex count() const = 0;
	constexpr static kIndex k_inf_count = -1;

	// 支持多通道数据，返回通道数目
	virtual kIndex channels() const = 0;

	// 清空数据
	virtual void clear() = 0;

	// 数据为空？
	virtual bool empty() const { return count() == 0; }

	// 返回axis维度的数据数量
	virtual kIndex length(kIndex axis) const = 0;

	// 返回axis维度的数据范围
	virtual kRange range(kIndex axis) const = 0;

	// 返回axis维度的步进，即dx, dy, dz...
	// 返回0表示步进非均匀，对应于散点数据
	virtual kReal step(kIndex axis) const = 0;
	constexpr static kReal k_nonuniform_step = 0;

	// 通过索引获取数据值
	// @idx: 大小为dim，各元素分表表示对应坐标轴的数据点索引
	virtual kReal value(kIndex idx[], kIndex channel) const = 0;

	// 参数同上，不同的是返回数据数组，含有各坐标轴的数据值
	virtual std::vector<kReal> point(kIndex idx[], kIndex channel) const = 0;

	// 通过坐标值获取数据值
	// @pt: 大小为dim，各元素分表表示对应坐标轴的坐标值
	virtual kReal value(kReal pt[], kIndex channel) const = 0;


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

	kReal value(kReal x, kIndex channel) const {
		assert(dim() == 1);
		return value(&x, channel);
	}

	kReal value(kReal x, kReal y, kIndex channel) const {
		assert(dim() == 2);
		kReal pt[2] = { x, y };
		return value(pt, channel);
	}


	// 是否连续数据？
	bool isContinued() const {
		return count() == k_inf_count;
	}

	// 是否散点数据？
	bool isScattered() const {
		return step(0) == k_nonuniform_step;
	}

	// 是否采样数据
	bool isSampled() const {
		return !isContinued() && !isScattered();
	}

};
