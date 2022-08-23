#pragma once
#include <vector>
#include <memory>

class KvData;

class KuDataUtil
{
public:

	using vectord = std::vector<double>;
	using matrixd = std::vector<vectord>;

	// 返回mat的最小列数和最大列数
	static std::pair<unsigned, unsigned> colsRange(const matrixd& mat);

	static matrixd transpose(const matrixd& mat);

	// 强制对齐mat各行的列数
	// @cols: 各行对齐到的列数，-1表示对齐到mat的最大列数，0表示对齐到mat的最小列数
	// @missingVal: 缺失数据的设定值
	static void forceAligned(matrixd& mat, unsigned cols = -1, double missingVal = std::numeric_limits<double>::quiet_NaN());

	static vectord column(const matrixd& mat, unsigned idx);

	static bool isEvenlySpaced(const vectord& v);


	static std::shared_ptr<KvData> makeSeries(const matrixd& mat);

	static std::shared_ptr<KvData> makeMatrix(const matrixd& mat);

	static std::shared_ptr<KvData> makeSampled1d(const matrixd& mat);

	static std::shared_ptr<KvData> makeSampled2d(const matrixd& mat);

	static std::shared_ptr<KvData> makeScattered(const matrixd& mat, unsigned dim);


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
	static std::vector<KeDataType> validTypes(const matrixd& mat, bool colMajor);

	// 把mat转换为type类型数据
	static std::shared_ptr<KvData> makeData(const matrixd& mat, KeDataType type);

	static const char* typeStr(KeDataType type);

private:
	KuDataUtil();
};