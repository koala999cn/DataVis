﻿#pragma once
#include <vector> // for std::pair
#include "kDsp.h" // for kReal
#include "KtInterval.h"

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

	constexpr static kReal k_inf_count = std::numeric_limits<kReal>::infinity();

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
	// 返回0表示步进未知，对应于连续数据
	// 返回inf表示步进非均匀，对应于散点数据
	virtual kReal step(kIndex axis) const = 0;

	constexpr static kReal k_unknown_step = 0;
	constexpr static kReal k_nonuniform_step = std::numeric_limits<kReal>::infinity();


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
		return count() != k_inf_count && step(0) != k_nonuniform_step;
	}

};
