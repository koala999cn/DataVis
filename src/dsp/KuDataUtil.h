#pragma once
#include <vector>
#include <memory>

class KvData;

class KuDataUtil
{
public:

	using vectord = std::vector<double>;
	using matrixd = std::vector<vectord>;

	// ����mat����С�������������
	static std::pair<unsigned, unsigned> colsRange(const matrixd& mat);

	static matrixd transpose(const matrixd& mat);

	// ǿ�ƶ���mat���е�����
	// @cols: ���ж��뵽��������-1��ʾ���뵽mat�����������0��ʾ���뵽mat����С����
	// @missingVal: ȱʧ���ݵ��趨ֵ
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

	// ���mat��ת������������
	// ����[0]Ϊ�Ƽ�����
	static std::vector<KeDataType> validTypes(const matrixd& mat, bool colMajor);

	// ��matת��Ϊtype��������
	static std::shared_ptr<KvData> makeData(const matrixd& mat, KeDataType type);

	static const char* typeStr(KeDataType type);

private:
	KuDataUtil();
};