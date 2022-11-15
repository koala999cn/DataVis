#pragma once
#include "KvDiscreted.h"


// 采样数据的抽象类

class KvSampled : public KvDiscreted
{
public:
	
	using KvDiscreted::size;

	// 调整第axis轴的采样参数
	virtual void reset(kIndex axis, kReal low, kReal step, kReal x0_ref = 0) = 0;

	// 通过索引获取数据值
	// @idx: 大小为dim，各元素分表表示对应坐标轴的数据点索引
	virtual kReal value(kIndex idx[], kIndex channel) const = 0;

	// 参数同上，不同的是返回数据数组，含有各坐标轴的数据值
	virtual std::vector<kReal> point(kIndex idx[], kIndex channel) const = 0;

	kIndex size() const override {
		kIndex sz = size(0);
		for (kIndex d = 1; d < dim(); d++)
			sz *= size(d);
		return sz;
	}

	kReal valueAt(kIndex n, kIndex channel) const override {
		return value(nToIndex(n).data(), channel);
	}

	std::vector<kReal> pointAt(kIndex n, kIndex channel) const override {
		return point(nToIndex(n).data(), channel);
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

	void nextIndex(kIndex idx[]) const {
		for (kIndex i = 0; i < dim(); i++) {
			if (++idx[i] < size(i))
				break;

			idx[i] = 0; // 进位
		}
	}

	// 返回第n个数据的索引
	std::vector<kIndex> nToIndex(kIndex n) const {

		// 快速通道
		if (dim() == 1) 
			return { n };
		else if (dim() == 2)
			return { n / size(1), n % size(1) };

		// 通用算法
		// TODO: test
		std::vector<kIndex> idx;
		for (kIndex i = dim(); i > 0; i--) {
			idx.insert(idx.begin(), n % size(i));
			n /= size(i);
		}

		idx.insert(idx.begin(), n);

		return idx;
	}

    auto sampleRate(kIndex axis) const {
        assert(step(axis) != 0);
        return static_cast<kReal>(1) / step(axis);
    }

    auto nyquistRate(kIndex axis) const {
        return sampleRate(axis) / 2;
    }

	// bps
	auto bytesPerSample() const { return sizeof(kReal) * channels(); }

	// N个采样点占据的内存大小(字节数)
	auto bytesOfSamples(kIndex N) const { return bytesPerSample() * N; }

	void validateIndex(kIndex idx[]) const {
		for (unsigned i = 0; i < dim(); i++)
			assert(idx[i] >= 0 && idx[i] < size(i));
	}
};
