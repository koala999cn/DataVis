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
	
	//static bool sameShape(const KvData& d1, const KvData& d2);

	//static void reshapeAs(KvData& from, const KvData& to);

	// shape���������һ�£���������
	// static std::shared_ptr<KvData> emptyLike();
	
	// shape���������һ�£�������������0
	// static std::shared_ptr<KvData> zeroLike();

	// shape�������������������ݾ�һ��
	// static std::shared_ptr<KvData> sameLike();


	/// ���ݷ��ʽӿ�

	struct KpValueGetter1d
	{
		std::function<double(unsigned ch, unsigned idx)> getter{ nullptr };
		unsigned channels{ 0 }; // ͨ����
		unsigned samples{ 0 }; // ÿ��ͨ����������
		const double* data{ nullptr }; // �ڴ���ʵ�ַ��nullptr��ʾ�����ڴ���ʣ�
		int channelStride{ 0 }; // ��ͨ����Ŀ�ȣ�double����������data�ǿ�ʱ��Ч
		int sampleStride{ 0 }; // ��������Ŀ�ȣ�double����������data�ǿ�ʱ��Ч

		// ��ȡָ��ͨ������
		auto fetchChannel(unsigned ch, unsigned offset, unsigned count) const {
			assert((offset < samples || count == 0) && count <= samples - offset);
			std::vector<double> buf(count);
			auto p = buf.data();
			auto last = offset + count;
			for (unsigned i = offset; i < last; i++)
				*p++ = getter(ch, i);
			return buf;
		}

		// ��ȡ����ͨ������
		auto fetch(unsigned offset, unsigned count) const {
			assert(offset < samples && count <= samples - offset);
			std::vector<double> buf(count * channels);
			auto p = buf.data();
			auto last = offset + count;
			for (unsigned i = offset; i < last; i++)
				for (unsigned ch = 0; ch < channels; ch++) // ��ͨ�����ݽ���洢
				    *p++ = getter(ch, i);
			return buf;
		}
	};


	static KpValueGetter1d valueGetter1d(const std::shared_ptr<KvData>& data);


	struct KpValueGetter2d
	{
		std::function<double(unsigned ch, unsigned row, unsigned col)> getter{ nullptr };
		unsigned channels{ 0 }; // ͨ����
		unsigned rows{ 0 }; // ÿ��ͨ��������
		unsigned cols{ 0 }; // ÿ��ͨ��������
		const double* data{ nullptr }; // �ڴ���ʵ�ַ��nullptr��ʾ�����ڴ���ʣ�
		int channelStride{ 0 }; // ��ͨ����Ŀ�ȣ�double����������data�ǿ�ʱ��Ч
		int rowStride{ 0 }; // ���м�Ŀ�ȣ�double����������data�ǿ�ʱ��Ч
		int colStride{ 0 }; // ���м�Ŀ�ȣ�double����������data�ǿ�ʱ��Ч

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
				for (unsigned ch = 0; ch < channels; ch++) // ��ͨ�����ݽ���洢
					*p++ = getter(ch, row, i);
			return buf;
		}
	};


	static KpValueGetter2d valueGetter2d(const std::shared_ptr<KvData>& data);

private:
	KuDataUtil() = default;
};
