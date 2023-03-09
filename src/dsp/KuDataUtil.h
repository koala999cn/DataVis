#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <assert.h>
#include "kDsp.h"

class KvData;
class KvDiscreted;
class KvSampled;

class KuDataUtil
{
public:
	using matrixd = std::vector<std::vector<kReal>>;

	enum KeDataType
	{
		k_series,
		k_matrix,
		k_scattered_1d,
		k_scattered_2d,
		k_sampled_1d,
		k_sampled_2d
	};

	// 检测mat可转换的数据类型
    // 返回[0]为推荐类型
	static std::vector<int> validTypes(const matrixd& mat, bool transpose);

	static std::shared_ptr<KvData> makeSeries(const matrixd& mat);

	static std::shared_ptr<KvData> makeMatrix(const matrixd& mat);

	// 多通道的一维采样数据
	static std::shared_ptr<KvData> makeSampled1d(const matrixd& mat);

	// 单通道的二维采样数据
	static std::shared_ptr<KvData> makeSampled2d(const matrixd& mat);

	static std::shared_ptr<KvData> makeScattered(const matrixd& mat, unsigned dim);

	// 把mat转换为type类型数据
	// 按照row-major读取mat数据
	static std::shared_ptr<KvData> makeData(const matrixd& mat, int type);

	static std::shared_ptr<KvData> cloneSampled1d(std::shared_ptr<const KvData> samp);

	static bool isMatrix(const KvData& d);
	
	static bool sameShape(const KvData& d1, const KvData& d2, bool sameStep);

	//static void reshapeAs(KvData& from, const KvData& to);

	// shape与采样参数一致，但数据未初始化
	static std::shared_ptr<KvDiscreted> shapeLike(const KvDiscreted& disc);
	
	// shape与采样参数一致，但数据内容置0
	// static std::shared_ptr<KvData> zeroLike();

	// shape、采样参数与数据内容均一致
	// static std::shared_ptr<KvData> sameLike();


	/// 多维数组索引支持函数

	// 获取返回采样数据的各维度尺寸
	static std::vector<kIndex> shape(const KvDiscreted& disc);

	// 将数组索引idx转换为顺序访问序号n
	static kIndex index2n(const std::vector<kIndex>& shape, const kIndex idx[]);

	// 将顺序访问序号n转换为数组索引
	static std::vector<kIndex> n2index(const std::vector<kIndex>& shape, kIndex n);

	// 计算idx的下一个索引，并写入idx
	static void nextIndex(const std::vector<kIndex>& shape, kIndex idx[]);

	// 计算idx的上一个索引，并写入idx
	static void prevIndex(const std::vector<kIndex>& shape, kIndex idx[]);

	// 测试函数
	static void indexTest();

	/// 数据访问接口

	struct KpValueGetter1d
	{
		std::function<kReal(unsigned ch, unsigned idx)> getter{ nullptr };
		unsigned channels{ 0 }; // 通道数
		unsigned samples{ 0 }; // 每个通道的样本数
		const kReal* data{ nullptr }; // 内存访问地址（nullptr表示不可内存访问）
		int channelStride{ 0 }; // 各通道间的跨度（double数），仅当data非空时有效
		int sampleStride{ 0 }; // 各样本间的跨度（double数），仅当data非空时有效

		// 提取指定通道数据
		auto fetchChannel(unsigned ch, unsigned offset, unsigned count) const {
			assert((offset < samples || count == 0) && count <= samples - offset);
			std::vector<kReal> buf(count);
			auto p = buf.data();
			auto last = offset + count;
			for (unsigned i = offset; i < last; i++)
				*p++ = getter(ch, i);
			return buf;
		}

		// 提取所有通道数据
		auto fetch(unsigned offset, unsigned count) const {
			assert(offset < samples && count <= samples - offset);
			std::vector<kReal> buf(count * channels);
			auto p = buf.data();
			auto last = offset + count;
			for (unsigned i = offset; i < last; i++)
				for (unsigned ch = 0; ch < channels; ch++) // 各通道数据交错存储
				    *p++ = getter(ch, i);
			return buf;
		}
	};


	static KpValueGetter1d valueGetter1d(const std::shared_ptr<const KvData>& data);


	struct KpValueGetter2d
	{
		std::function<kReal(unsigned ch, unsigned ix, unsigned iy)> getter{ nullptr };
		unsigned channels{ 0 }; // 通道数
		unsigned xsize{ 0 }; // 每个通道的行数
		unsigned ysize{ 0 }; // 每个通道的列数
		const kReal* data{ nullptr }; // 内存访问地址（nullptr表示不可内存访问）
		int channelStride{ 0 }; // 各通道间的跨度（double数），仅当data非空时有效
		int xstride{ 0 }; // 各行间的跨度（double数），仅当data非空时有效
		int ystride{ 0 }; // 各列间的跨度（double数），仅当data非空时有效

		auto fetchXOfChannel(unsigned ch, unsigned ix) const {
			std::vector<kReal> buf(ysize);
			auto p = buf.data();
			for (unsigned iy = 0; iy < ysize; iy++)
				*p++ = getter(ch, ix, iy);
			return buf;
		}

		auto fetchX(unsigned ix) const {
			std::vector<kReal> buf(ysize * channels);
			auto p = buf.data();
			for (unsigned iy = 0; iy < ysize; iy++)
				for (unsigned ch = 0; ch < channels; ch++) // 各通道数据交错存储
					*p++ = getter(ch, ix, iy);
			return buf;
		}
	};


	static KpValueGetter2d valueGetter2d(const std::shared_ptr<const KvData>& data);

	struct KpPointGetter1d
	{
		std::function<std::vector<kReal>(unsigned idx)> getter;
		unsigned size;
	};

	struct KpPointGetter2d
	{
		std::function<std::vector<kReal>(unsigned ix, unsigned iy)> getter;
		unsigned xsize, ysize;
	};

	static bool hasPointGetter2d(const std::shared_ptr<const KvDiscreted>& disc);

	// 计算disc每个通道有多少条pointGetter1d
	static unsigned pointGetter1dCount(const std::shared_ptr<const KvDiscreted>& disc);

	// 计算disc每个通道有多少条pointGetter2d
	static unsigned pointGetter2dCount(const std::shared_ptr<const KvDiscreted>& disc);

	// 获取disc的第ch通道的第idx条pointGetter1d
	static KpPointGetter1d pointGetter1dAt(const std::shared_ptr<const KvDiscreted>& disc, unsigned ch, unsigned idx);

	// 获取disc的第ch通道的第idx条pointGetter2d
	static KpPointGetter2d pointGetter2dAt(const std::shared_ptr<const KvDiscreted>& disc, unsigned ch, unsigned idx);
	

private:
	KuDataUtil() = default;
};
