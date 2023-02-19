#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <assert.h>

class KvData;

class KuDataUtil
{
public:
	using matrixd = std::vector<std::vector<double>>;

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
	static std::vector<int> validTypes(const matrixd& mat, bool colMajor);

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

	static std::shared_ptr<KvData> cloneSampled1d(std::shared_ptr<KvData> samp);

	static bool isMatrix(const KvData& d);
	
	//static bool sameShape(const KvData& d1, const KvData& d2);

	//static void reshapeAs(KvData& from, const KvData& to);

	// shape与采样参数一致，但无数据
	// static std::shared_ptr<KvData> emptyLike();
	
	// shape与采样参数一致，但数据内容置0
	// static std::shared_ptr<KvData> zeroLike();

	// shape、采样参数与数据内容均一致
	// static std::shared_ptr<KvData> sameLike();


	/// 数据访问接口

	struct KpValueGetter1d
	{
		std::function<double(unsigned ch, unsigned idx)> getter{ nullptr };
		unsigned channels{ 0 }; // 通道数
		unsigned samples{ 0 }; // 每个通道的样本数
		const double* data{ nullptr }; // 内存访问地址（nullptr表示不可内存访问）
		int channelStride{ 0 }; // 各通道间的跨度（double数），仅当data非空时有效
		int sampleStride{ 0 }; // 各样本间的跨度（double数），仅当data非空时有效

		// 提取指定通道数据
		auto fetchChannel(unsigned ch, unsigned offset, unsigned count) const {
			assert((offset < samples || count == 0) && count <= samples - offset);
			std::vector<double> buf(count);
			auto p = buf.data();
			auto last = offset + count;
			for (unsigned i = offset; i < last; i++)
				*p++ = getter(ch, i);
			return buf;
		}

		// 提取所有通道数据
		auto fetch(unsigned offset, unsigned count) const {
			assert(offset < samples && count <= samples - offset);
			std::vector<double> buf(count * channels);
			auto p = buf.data();
			auto last = offset + count;
			for (unsigned i = offset; i < last; i++)
				for (unsigned ch = 0; ch < channels; ch++) // 各通道数据交错存储
				    *p++ = getter(ch, i);
			return buf;
		}
	};


	static KpValueGetter1d valueGetter1d(const std::shared_ptr<KvData>& data);


	struct KpValueGetter2d
	{
		std::function<double(unsigned ch, unsigned row, unsigned col)> getter{ nullptr };
		unsigned channels{ 0 }; // 通道数
		unsigned rows{ 0 }; // 每个通道的行数
		unsigned cols{ 0 }; // 每个通道的列数
		const double* data{ nullptr }; // 内存访问地址（nullptr表示不可内存访问）
		int channelStride{ 0 }; // 各通道间的跨度（double数），仅当data非空时有效
		int rowStride{ 0 }; // 各行间的跨度（double数），仅当data非空时有效
		int colStride{ 0 }; // 各列间的跨度（double数），仅当data非空时有效

		auto fetchRowOfChannel(unsigned ch, unsigned row) const {
			std::vector<double> buf(cols);
			auto p = buf.data();
			for (unsigned i = 0; i < cols; i++)
				*p++ = getter(ch, row, i);
			return buf;
		}

		auto fetchRow(unsigned row) const {
			std::vector<double> buf( cols * channels);
			auto p = buf.data();
			for (unsigned i = 0; i < cols; i++)
				for (unsigned ch = 0; ch < channels; ch++) // 各通道数据交错存储
					*p++ = getter(ch, row, i);
			return buf;
		}
	};


	static KpValueGetter2d valueGetter2d(const std::shared_ptr<KvData>& data);

private:
	KuDataUtil() = default;
};
