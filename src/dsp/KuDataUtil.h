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

	// ���mat��ת������������
    // ����[0]Ϊ�Ƽ�����
	static std::vector<int> validTypes(const matrixd& mat, bool colMajor);

	static std::shared_ptr<KvData> makeSeries(const matrixd& mat);

	static std::shared_ptr<KvData> makeMatrix(const matrixd& mat);

	// ��ͨ����һά��������
	static std::shared_ptr<KvData> makeSampled1d(const matrixd& mat);

	// ��ͨ���Ķ�ά��������
	static std::shared_ptr<KvData> makeSampled2d(const matrixd& mat);

	static std::shared_ptr<KvData> makeScattered(const matrixd& mat, unsigned dim);

	// ��matת��Ϊtype��������
	// ����row-major��ȡmat����
	static std::shared_ptr<KvData> makeData(const matrixd& mat, int type);

	static std::shared_ptr<KvData> cloneSampled1d(std::shared_ptr<KvData> samp);

	static bool isMatrix(const KvData& d);

private:
	KuDataUtil() = default;
};
