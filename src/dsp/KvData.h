#pragma once
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

	// 包含的数据总数
	virtual kIndex count() const = 0;

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
	// 返回=0表示步进未知，返回inf表示步进非均匀
	virtual kReal step(kIndex axis) const = 0;

	constexpr static kReal k_unknown_step = 0;
	constexpr static kReal k_nonuniform_step = std::numeric_limits<kReal>::infinity();
};
