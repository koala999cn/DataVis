#pragma once
#include <vector> // for std::pair
#include "kDsp.h" // for kReal

using kRange = std::pair<kReal, kReal>;

// 所有数据源的基类. 仅提供一个接口，即获取数据维度的方法
class KvData
{
public:

	// 数据的维度
	virtual unsigned dim() = 0; 

	// 返回第axis轴的数据范围
	virtual kRange range(int axis) = 0;

	// 返回第axis轴的步进，即dx, dy, dz...
	// 返回=0表示步进未知，返回inf表示步进非均匀
	virtual kReal step(int axis) const = 0;

	constexpr static kReal k_unknown_step = 0;
	constexpr static kReal k_nonuniform_step = std::numeric_limits<kReal>::infinity();

	// 清空数据
	virtual void clear() = 0;
};
