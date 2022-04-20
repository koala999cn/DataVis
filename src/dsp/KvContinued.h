#pragma once
#include "KvData.h"


class KvContinued : public KvData
{
public:

	// 通过坐标值获取数据值
	// @pt: 大小为dim，各元素分表表示对应坐标轴的坐标值
	virtual kReal value(kReal pt[], kIndex channel) const = 0;


	/// 几个helper函数

	kReal value(kReal x, kIndex channel) const {
		assert(dim() == 1);
		return value(&x, channel);
	}

	kReal value(kReal x, kReal y, kIndex channel) const {
		assert(dim() == 2);
		kReal pt[2] = { x, y };
		return value(pt, channel);
	}

};