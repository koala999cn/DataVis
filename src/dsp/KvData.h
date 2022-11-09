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
	virtual kIndex size() const = 0;
	constexpr static kIndex k_inf_size = -1;

	// 支持多通道数据，返回通道数目
	virtual kIndex channels() const = 0;

	virtual kIndex count() const {
		return size() * channels();
	}

	virtual kReal length(kIndex axis) const {
		return range(axis).length();
	}

	// 返回axis维度的数据范围
	virtual kRange range(kIndex axis) const = 0;

	// 第channel通道的最大最小值
	virtual kRange valueRange(kIndex channel) const = 0;

	// 所有通道的最大最小值
	virtual kRange valueRange() const {
		auto r = valueRange(0);
		for (kIndex c = 1; c < channels(); c++) {
			auto rc = valueRange(c);
			if (rc.low() < r.low())
				r.resetLow(rc.low());
			if (rc.high() > r.high())
				r.resetHigh(rc.high());
		}

		return r;
	}


	// 是否连续数据？
	bool isContinued() const { return size() == k_inf_size; }

	// 是否离散数据？
	bool isDiscreted() const { return !isContinued(); }
};
