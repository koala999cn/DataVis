#pragma once
#include <memory>
#include <vector>

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

private:
	KuDataUtil() = default;
};
